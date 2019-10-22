//
// Created by Andrey Syvrachev on 22.10.2019.
//

#ifndef TGVOIPCONTEST_PCMRESAMPLER_H
#define TGVOIPCONTEST_PCMRESAMPLER_H

#include <IPCMSource.h>
#include <vector>


class PCMResampler: public IPCMSource {
public:
    PCMResampler(IPCMSource& source, int from, int to);
    const short *Samples() override;
    unsigned long Size() override;
private:
    std::vector<short> _samples;
};


#endif //TGVOIPCONTEST_PCMRESAMPLER_H
