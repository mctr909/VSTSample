#pragma once

// 自作VST用のインクルードファイル
#include "myvst3define.h"

class Channel {
public:
    double Vol = 0.0;
    double Exp = 0.0;
    double PanL = 0.0;
    double PanR = 0.0;
    double Pitch = 1.0;
    int BendRange = 2;
    int WriteIndex = 0;
    double* pWaveL = NULL;
    double* pWaveR = NULL;
    Channel() {
        pWaveL = (double*)calloc(BUFF_SIZE, sizeof(double));
        pWaveR = (double*)calloc(BUFF_SIZE, sizeof(double));
    }
    ~Channel() {
        free(pWaveL);
        free(pWaveR);
    }
    void Reset() {
        Vol = 100 / 127.0;
        Exp = 100 / 127.0;
        PanL = 0.707;
        PanR = 0.707;
        Pitch = 1.0;
        BendRange = 2;
    }
};

Channel* gppChannels[CHANNEL_COUNT] = { NULL };
