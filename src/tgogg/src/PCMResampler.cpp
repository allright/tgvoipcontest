//
// Created by Andrey Syvrachev on 22.10.2019.
//

#include "PCMResampler.h"

PCMResampler::PCMResampler(IPCMSource& source, int from, int to) {
//
//    ost->swr_ctx = swr_alloc();
//    if (!ost->swr_ctx) {
//        throw StrException( "Could not allocate resampler context\n");
//    }
//
//    /* set options */
//    av_opt_set_int       (ost->swr_ctx, "in_channel_count",   c->channels,       0);
//    av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     c->sample_rate,    0);
//    av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
//
//    av_opt_set_int       (ost->swr_ctx, "out_channel_count",  c->channels,       0);
//    av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
//    av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);
//
//    /* initialize the resampling context */
//    if ((ret = swr_init(ost->swr_ctx)) < 0) {
//        throw StrException("Failed to initialize the resampling context\n");
//    }
}

const short *PCMResampler::Samples() {
    return _samples.data();
}

unsigned long PCMResampler::Size() {
    return _samples.size();
}