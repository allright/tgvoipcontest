//
// Created by Andrey Syvrachev on 18.10.2019.
//

#ifndef TGVOIPRATE_LPF_H
#define TGVOIPRATE_LPF_H


class LPF {
public:
    float ProcessSample(short sample);

private:
    float _state = 0;
};


#endif //TGVOIPRATE_LPF_H
