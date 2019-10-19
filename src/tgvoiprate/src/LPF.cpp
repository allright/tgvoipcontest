//
// Created by Andrey Syvrachev on 18.10.2019.
//

#include "LPF.h"

const float cAlpha = 0.015;

float LPF::ProcessSample(short sample) {
    _state = _state * (1 - cAlpha) + sample *cAlpha;
    return _state;
}