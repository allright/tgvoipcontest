//
// Created by Andrey Syvrachev on 18.10.2019.
//

#include "File.h"
#include "StrException.h"

static unsigned long fileSize(FILE* file) {
    fseek(file, 0L, SEEK_END);
    auto size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

const auto cWavHeaderSize = 44;

File::File(const char *name) {

   // printf("[%p] File::File(%s)\n",this,name);
    FILE* f = fopen(name,"rb");
    if (f == nullptr) {
        throw StrException("can not open file '%s'",name);
    }

    auto size = fileSize(f);
    //printf("[%p] File::File(%s) size = %lu\n",this,name, size);
    _samples.resize(size);
    if (size < cWavHeaderSize) {
        throw StrException("no wav header in file '%s'",name);
    }
    size -= cWavHeaderSize;
    fseek(f, cWavHeaderSize, SEEK_SET);
    if (cWavHeaderSize != ftell(f)) {
        throw StrException("error skip wav header in file '%s'",name);
    }

    auto readSz = fread(_samples.data(),1,size,f);
    if (readSz != size) {
        throw StrException("error reading file '%s'",name);
    }
    fclose(f);
}

//File::~File() {
//    printf("[%p] File::~File()\n",this);
//}

const short *File::Samples() {
    return _samples.data();
}

unsigned long File::Size() {
    return _samples.size()/2;
}
