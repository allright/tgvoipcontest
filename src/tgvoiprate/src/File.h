//
// Created by Andrey Syvrachev on 18.10.2019.
//

#ifndef TGVOIPRATE_FILE_H
#define TGVOIPRATE_FILE_H

#include <vector>
#include "IPCMSource.h"

class File: public IPCMSource {
public:
    File(const char* name);
  //  ~File();
    const short *Samples() override ;
    unsigned long Size() override;
private:
    std::vector<short> _samples;
};


#endif //TGVOIPRATE_FILE_H
