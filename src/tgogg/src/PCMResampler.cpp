//
// Created by Andrey Syvrachev on 22.10.2019.
//

#include "PCMResampler.h"
#include "StrException.h"

extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

PCMResampler::PCMResampler(std::shared_ptr<IPCMSource> &source, int from, int to) {
    if (from != 48000 || to != 16000)
        throw StrException("only 48000->16000 supported");
    _samples.resize(source->Size() / 3);
   // memcpy(_samples.data(), source->Samples(), source->Size() / 2);

    struct SwrContext *swr_ctx;


    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        throw StrException( "Could not allocate resampler context\n");
    }
////
////    /* set options */
    av_opt_set_int       (swr_ctx, "in_channel_count",   1,       0);
    av_opt_set_int       (swr_ctx, "in_sample_rate",     from,    0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);

    av_opt_set_int       (swr_ctx, "out_channel_count",  1,       0);
    av_opt_set_int       (swr_ctx, "out_sample_rate",    to,    0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt",     AV_SAMPLE_FMT_S16,     0);
////
////    /* initialize the resampling context */
    if (swr_init(swr_ctx) < 0) {
        throw StrException("Failed to initialize the resampling context\n");
    }
//
    uint8_t *out[1];
    out[0] = (uint8_t*)_samples.data();

    uint8_t *in[1];
    in[0] = (uint8_t*)source->Samples();
    int ret = swr_convert(swr_ctx, (uint8_t **)&out, (int)_samples.size(), (const uint8_t **)&in, (int)source->Size());
    if (ret < 0) {
        throw StrException("swr_convert failed\n");
    }
//
    swr_free(&swr_ctx);
}

const short *PCMResampler::Samples() {
    return _samples.data();
}

unsigned long PCMResampler::Size() {
    return _samples.size();
}