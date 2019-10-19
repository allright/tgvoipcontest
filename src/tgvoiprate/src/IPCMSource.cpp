//
// Created by Andrey Syvrachev on 18.10.2019.
//
#include "IPCMSource.h"
#include "File.h"

std::shared_ptr<IPCMSource> IPCMSource::OpenWavFile(const char *path) {
    return std::shared_ptr<IPCMSource>(new File(path));
}

