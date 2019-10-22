//
// Created by Andrey Syvrachev on 22.10.2019.
//

#ifndef TGVOIPCONTEST_IPCMDEST_H
#define TGVOIPCONTEST_IPCMDEST_H

#include <memory>

class IPCMDest {
public:
    virtual void Write(const short* data, unsigned long samples) = 0;

    static std::shared_ptr<IPCMDest> openOggWriter(const char* name);
};


#endif //TGVOIPCONTEST_IPCMDEST_H
