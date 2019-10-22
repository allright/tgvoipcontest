//
// Created by Andrey Syvrachev on 22.10.2019.
//

#ifndef TGVOIPCONTEST_TGOGGWRITER_H
#define TGVOIPCONTEST_TGOGGWRITER_H

#include <IPCMDest.h>

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

// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;
    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;
    AVFrame *frame;
    AVFrame *tmp_frame;
    float t, tincr, tincr2;
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

class TGOggWriter: public IPCMDest {
public:
    TGOggWriter(const char* name);
    ~TGOggWriter();

    void Write(const short* data, unsigned long samples);

private:
    AVFormatContext *oc;
    OutputStream audio_st = { 0 };

};


#endif //TGVOIPCONTEST_TGOGGWRITER_H
