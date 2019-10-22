//
// Created by Andrey Syvrachev on 22.10.2019.
//

#include "TGOggWriter.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
}

#include <StrException.h>

TGOggWriter::TGOggWriter(const char* name) {
    AVFormatContext *oc;

    avformat_alloc_output_context2(&oc, NULL, NULL, name);
    if (!oc)
        throw StrException("ogg format not supported");
}

void TGOggWriter::Write(const short* data, unsigned long samples) {
    printf("TGOggWriter::Write(num = %zu)\n",samples);
}


std::shared_ptr<IPCMDest> IPCMDest::openOggWriter(const char* name) {
    return std::shared_ptr<IPCMDest>(new TGOggWriter(name));
}
