//
// Created by Andrey Syvrachev on 22.10.2019.
//

#include "TGOggWriter.h"
#include <StrException.h>
#include <algorithm>

/* Add an output stream. */
static void add_stream(OutputStream *ost, AVFormatContext *oc,
                       AVCodec **codec,
                       enum AVCodecID codec_id)
{
    AVCodecContext *c;
    int i;
    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        throw StrException("Could not find encoder for '%s'\n",avcodec_get_name(codec_id));
    }
    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        throw StrException("Could not allocate stream\n");
    }
    ost->st->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        throw StrException("Could not alloc an encoding context\n");
    }
    ost->enc = c;

    assert((*codec)->type == AVMEDIA_TYPE_AUDIO);
    c->sample_fmt = AV_SAMPLE_FMT_FLTP;
    c->bit_rate = 64000;
    c->sample_rate = 48000;
    c->channels = 1;
    c->channel_layout = AV_CH_LAYOUT_MONO;
    ost->st->time_base = (AVRational) {1, c->sample_rate};

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

static AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                                  uint64_t channel_layout,
                                  int sample_rate, int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    int ret;
    if (!frame) {
        throw StrException( "Error allocating an audio frame\n");
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;
    if (nb_samples) {
        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            throw StrException("Error allocating an audio buffer\n");
        }
    }
    return frame;
}

static void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;
    c = ost->enc;

    /* open it */
    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);

    if (ret < 0) {
        throw StrException( "Could not open audio codec: %s\n", av_err2str(ret));
    }

    /* init signal generator */
    ost->t     = 0;
    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;
    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;

    assert(nb_samples == 960);

    ost->frame     = alloc_audio_frame(c->sample_fmt, c->channel_layout, c->sample_rate, nb_samples);
    ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout, c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        throw StrException("Could not copy the stream parameters\n");
    }

    /* create resampler context */
    ost->swr_ctx = swr_alloc();
    if (!ost->swr_ctx) {
        throw StrException( "Could not allocate resampler context\n");
    }

    /* set options */
    av_opt_set_int       (ost->swr_ctx, "in_channel_count",   c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);

    av_opt_set_int       (ost->swr_ctx, "out_channel_count",  c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);

    /* initialize the resampling context */
    if ((ret = swr_init(ost->swr_ctx)) < 0) {
        throw StrException("Failed to initialize the resampling context\n");
    }
}

static void close_stream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
 //   sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

#define STREAM_DURATION   10.0
/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
static AVFrame *get_audio_frame(OutputStream *ost)
{
    AVFrame *frame = ost->tmp_frame;
    int j, i, v;
    int16_t *q = (int16_t*)frame->data[0];
    /* check if we want to generate more frames */
    if (av_compare_ts(ost->next_pts, ost->enc->time_base,
                      STREAM_DURATION, (AVRational){ 1, 1 }) > 0)
        return NULL;
    for (j = 0; j <frame->nb_samples; j++) {
        v = (int)(sin(ost->t) * 10000);
        for (i = 0; i < ost->enc->channels; i++)
            *q++ = v;
        ost->t     += ost->tincr;
        ost->tincr += ost->tincr2;
    }
    frame->pts = ost->next_pts;
    ost->next_pts  += frame->nb_samples;
    return frame;
}

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;
    /* Write the compressed frame to the media file. */
    log_packet(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

/*
 * encode one audio frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_audio_frame(AVFormatContext *oc, OutputStream *ost,AVFrame *frame)
{
    AVCodecContext *c;
    AVPacket pkt = { 0 }; // data and size must be 0;

    int ret;
    int got_packet;
    int dst_nb_samples;
    av_init_packet(&pkt);
    c = ost->enc;
    if (frame) {
        /* convert samples from native format to destination codec format, using the resampler */
        /* compute destination number of samples */
        dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
                                        c->sample_rate, c->sample_rate, AV_ROUND_UP);
        av_assert0(dst_nb_samples == frame->nb_samples);
        /* when we pass a frame to the encoder, it may keep a reference to it
         * internally;
         * make sure we do not overwrite it here
         */
        ret = av_frame_make_writable(ost->frame);
        if (ret < 0)
            throw StrException("av_frame_make_writable ret = %d",ret);
        /* convert to destination format */
        ret = swr_convert(ost->swr_ctx,
                          ost->frame->data, dst_nb_samples,
                          (const uint8_t **)frame->data, frame->nb_samples);
        if (ret < 0) {
            throw StrException("Error while converting\n");
        }
        frame = ost->frame;
        frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
        ost->samples_count += dst_nb_samples;
    }

    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        throw StrException( "Error encoding audio frame: %s\n", av_err2str(ret));
    }
    if (got_packet) {
        ret = write_frame(oc, &c->time_base, ost->st, &pkt);
        if (ret < 0) {
            throw StrException( "Error while writing audio frame: %s\n", av_err2str(ret));
        }
    }
    return (frame || got_packet) ? 0 : 1;
}

TGOggWriter::TGOggWriter(const char* name) {

    avformat_alloc_output_context2(&oc, NULL, NULL, name);
    if (!oc)
        throw StrException("ogg format not supported");

    AVOutputFormat *fmt = oc->oformat;
    AVCodec *audio_codec;

    add_stream(&audio_st, oc, &audio_codec, AV_CODEC_ID_OPUS);

    AVDictionary *opt = NULL;
    open_audio(oc, audio_codec, &audio_st, opt);

    av_dump_format(oc, 0, name, 1);

    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&oc->pb, name, AVIO_FLAG_WRITE);
        if (ret < 0) {
            throw StrException( "Could not open '%s': %s\n", name, av_err2str(ret));
        }
    }

    /* Write the stream header, if any. */
    int ret = avformat_write_header(oc, &opt);
    if (ret < 0) {
        throw StrException( "Error occurred when opening output file: %s\n",av_err2str(ret));
    }
//
//    int  encode_audio = 1;
//    while (encode_audio) {
//        /* select the stream to encode */
//        AVFrame* frame = get_audio_frame(&audio_st);
//        encode_audio = !write_audio_frame(oc, &audio_st, frame);
//    }

}

TGOggWriter::~TGOggWriter() {
    /* Write the trailer, if any. The trailer must be written before you
 * close the CodecContexts open when you wrote the header; otherwise
 * av_write_trailer() may try to use memory that was freed on
 * av_codec_close(). */
    av_write_trailer(oc);

    close_stream(oc, &audio_st);
    AVOutputFormat *fmt = oc->oformat;
    if (!(fmt->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&oc->pb);
    /* free the stream */
    avformat_free_context(oc);
}


void TGOggWriter::Write(const short* data, unsigned long samples) {
    printf("TGOggWriter::Write(num = %zu)\n",samples);
    AVFrame *frame = audio_st.tmp_frame;
    assert(frame->nb_samples == 960);
    int16_t *q = (int16_t*)frame->data[0];
    unsigned long len = std::min(samples,(unsigned long)frame->nb_samples);
    memcpy(q,data,samples*2);
    write_audio_frame(oc, &audio_st, frame);
}


std::shared_ptr<IPCMDest> IPCMDest::openOggWriter(const char* name) {
    return std::shared_ptr<IPCMDest>(new TGOggWriter(name));
}
