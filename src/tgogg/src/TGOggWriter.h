//
// Created by Andrey Syvrachev on 22.10.2019.
//

#ifndef TGVOIPCONTEST_TGOGGWRITER_H
#define TGVOIPCONTEST_TGOGGWRITER_H

#include <IPCMDest.h>

class TGOggWriter: public IPCMDest {
public:
    TGOggWriter(const char* name);

    void Write(const short* data, unsigned long samples);
};


#endif //TGVOIPCONTEST_TGOGGWRITER_H
