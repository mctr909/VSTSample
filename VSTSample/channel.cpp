#include <stdlib.h>
#include <math.h>
#include "channel.h"

namespace Steinberg {
    namespace Vst {
        Channel* Channel::List[CHANNEL_COUNT] = { NULL };

        Channel::Channel() {
            pWaveL = (double*)calloc(BUFF_SIZE, sizeof(double));
            pWaveR = (double*)calloc(BUFF_SIZE, sizeof(double));
            Reset();
            mCurAmp = Vol * Exp;
            mCurPanL = PanL;
            mCurPanR = PanR;
        }

        Channel::~Channel() {
            free(pWaveL);
            free(pWaveR);
        }

        void Channel::Reset() {
            Vol = 100 / 127.0;
            Exp = 100 / 127.0;
            PanL = 0.702;
            PanR = 0.702;
            Pitch = 1.0;
            BendRange = 2;
        }

        void Channel::CtrlChange(uint8 type, int8 data1, int8 data2) {
            switch (type) {
            case 7:
                Vol = data1 / 127.0;
                break;
            case 10:
                PanL = cos(1.57 * (data1 + 0.5) / 128.0);
                PanR = sin(1.57 * (data1 + 0.5) / 128.0);
                break;
            case 11:
                Exp = data1 / 127.0;
                break;
            default:
                break;
            }
        }

        void Channel::Step(ProcessData& data) {
            Sample32* outL = data.outputs[0].channelBuffers32[0];
            Sample32* outR = data.outputs[0].channelBuffers32[1];

            for (int32 i = 0; i < data.numSamples; i++) {
                outL[i] += pWaveL[WriteIndex] * mCurAmp * mCurPanL;
                outR[i] += pWaveR[WriteIndex] * mCurAmp * mCurPanR;

                mCurAmp += (Vol * Exp - mCurAmp) * 0.01;
                mCurPanL += (PanL - mCurPanL) * 0.01;
                mCurPanR += (PanR - mCurPanR) * 0.01;

                pWaveL[WriteIndex] = 0.0;
                pWaveR[WriteIndex] = 0.0;
                WriteIndex++;
                if (BUFF_SIZE <= WriteIndex) {
                    WriteIndex -= BUFF_SIZE;
                }
            }
        }
    }
}
