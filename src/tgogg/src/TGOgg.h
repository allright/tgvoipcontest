//
// Created by Andrey Syvrachev on 21.10.2019.
//

#ifndef TGVOIPCONTEST_TGOGG_H
#define TGVOIPCONTEST_TGOGG_H

#include <IPCMSource.h>
#include <vector>

class TGOgg: public IPCMSource {
public:
    TGOgg(const char* name);
    const short *Samples() override { return _samples.data(); }
    unsigned long Size() override { return _samples.size() / 2; }
private:
    std::vector<short> _samples;
};


#endif //TGVOIPCONTEST_TGOGG_H
