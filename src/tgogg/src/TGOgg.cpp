//
// Created by Andrey Syvrachev on 21.10.2019.
//

#include "TGOgg.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
}

#include <StrException.h>

static int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type, const char *src_filename) {
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n", av_get_media_type_string(type), src_filename);
        return ret;
    } else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];

        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n", av_get_media_type_string(type));
            return ret;
        }

        if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n", av_get_media_type_string(type));
            return ret;
        }
    }
    return 0;
}


static int decode_packet(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt, int *got_frame, int cached, std::vector<short>& outSamples) {
    /* decode audio frame */
    int ret = avcodec_decode_audio4(ctx, frame, got_frame, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error decoding audio frame\n");
        return ret;
    }
    printf("r = (%u): %u : %u\n", ret, pkt->size, pkt->duration);

    if (*got_frame) {
        int data_size = av_get_bytes_per_sample(ctx->sample_fmt);

        printf("audio_frame%s nb_samples:%d pts:%s %u: %u\n",
               cached ? "(cached)" : "", frame->nb_samples,
               av_ts2timestr(frame->pts, &ctx->time_base), data_size, ctx->channels);

        for (auto i = 0; i < frame->nb_samples; i++)
            for (auto ch = 0; ch < ctx->channels; ch++) {
                uint8_t * p = frame->data[ch] + data_size*i;
                float *t = (float*) p;
                short s = (short)((*t)*32767.0); //((s >> 8)&0xFF) | ((s << 8)&0xFF00)
                outSamples.push_back(s);
                printf("%f: %d ",*t,s );
//                fwrite(p, 1, data_size, f);
            }
        printf("\n");

    }


    return ret;
}


static void decode(const char *name, std::vector<short>& outSamples) {
    av_register_all();
    printf("name = %s\n", name);

    AVFormatContext *fmt_ctx = NULL;
    if (avformat_open_input(&fmt_ctx, name, NULL, NULL) < 0) {
        throw StrException("Could not open source file '%s'", name);
    }

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        throw StrException("Could not find stream information\n");
    }

    int audio_stream_idx = -1;
    AVStream *audio_stream = NULL;
    AVCodecContext *audio_dec_ctx;
    if (open_codec_context(&audio_stream_idx, fmt_ctx, AVMEDIA_TYPE_AUDIO, name) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
        audio_dec_ctx = audio_stream->codec;
    }

    int nb_planes = av_sample_fmt_is_planar(audio_dec_ctx->sample_fmt) ? audio_dec_ctx->channels : 1;
    printf("nb_planes = %u\n", nb_planes);
    uint8_t **audio_dst_data = (uint8_t **) av_mallocz(sizeof(uint8_t *) * nb_planes);
    if (!audio_dst_data) {
        throw StrException("Could not allocate audio data buffers\n");
    }

    av_dump_format(fmt_ctx, 0, name, 0);

    if (!audio_stream) {
        throw StrException("Could not find audio stream in the input, aborting\n");
    }

    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        throw StrException("Could not allocate frame\n");
    }

  //  FILE* out = fopen("ooo.pcm","wb");

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;



    int got_frame;

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0)
        decode_packet(audio_dec_ctx, frame, &pkt, &got_frame, 0, outSamples);

    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(audio_dec_ctx, frame, &pkt, &got_frame, 1, outSamples);
    } while (got_frame);


 //   fclose(out);
    if (audio_dec_ctx)
        avcodec_close(audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_free(frame);
    av_free(audio_dst_data);
}

TGOgg::TGOgg(const char *name) {
    decode(name,_samples);
}

std::shared_ptr<IPCMSource> IPCMSource::openOggFile(const char *name) {
    return std::shared_ptr<IPCMSource>(new TGOgg(name));
}
