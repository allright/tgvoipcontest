//
// Created by Andrey Syvrachev on 18.10.2019.
//

#ifndef TGVOIPRATE_IPCMSOURCE_H
#define TGVOIPRATE_IPCMSOURCE_H

#include <memory>

class IPCMSource {
public:
    virtual const short *Samples() = 0;
    virtual unsigned long Size() = 0;
    ~IPCMSource(){}

    static std::shared_ptr<IPCMSource> openOggFile(const char* name);
};


#endif //TGVOIPRATE_IPCMSOURCE_H
