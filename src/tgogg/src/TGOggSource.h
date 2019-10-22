//
// Created by Andrey Syvrachev on 21.10.2019.
//

#ifndef TGVOIPCONTEST_TGOGGSOURCE_H
#define TGVOIPCONTEST_TGOGGSOURCE_H

#include <IPCMSource.h>
#include <vector>

class TGOggSource: public IPCMSource {
public:
    TGOggSource(const char* name);
    const short *Samples() override { return _samples.data(); }
    unsigned long Size() override { return _samples.size() / 2; }
private:
    std::vector<short> _samples;
};


#endif //TGVOIPCONTEST_TGOGGSOURCE_H
