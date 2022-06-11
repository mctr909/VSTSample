#pragma once

// 自作VST用のインクルードファイル
#include "myvst3define.h"

enum struct SAMPLER_STATE {
    FREE,
    RESERVED,
    ACTIVE,
    HOLD,
    RELEASE,
    PURGE
};

class Sampler {
public:
    SAMPLER_STATE state = SAMPLER_STATE::FREE;
    int channelNumber = 0;
    int noteNumber = 0;
    double delta = 1.0;
    double gain = 1.0;
    double re = 0.0;
    double im = 0.01;
};

Sampler* gppSamplers[SAMPLER_COUNT] = { NULL };
