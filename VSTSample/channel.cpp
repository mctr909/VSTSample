#include <stdlib.h>
#include "channel.h"

namespace Steinberg {
    namespace Vst {
        ProcessSetup* Channel::Setup = NULL;
        Channel* Channel::List[CHANNEL_COUNT] = { NULL };
        ParamValue Channel::MasterVolume = 0.75;

        Channel::Channel(uint8 number) {
            Number = number;
            pInst = DLS::Instance->GetInst(9 == number, 0, 0, 0);
            pWaveL = (double*)calloc(BUFF_SIZE, sizeof(double));
            pWaveR = (double*)calloc(BUFF_SIZE, sizeof(double));
            Reset();
        }

        Channel::~Channel() {
            free(pWaveL);
            free(pWaveR);
        }

        void Channel::Reset() {
            Vol = 100 / 127.0;
            Exp = 100 / 127.0;
            mPanL = 0.702;
            mPanR = 0.702;
            Pitch = 1.0;
            BendRange = 2;

            mCurAmp = Vol * Exp;
            CurPanL = mPanL;
            CurPanR = mPanR;
        }

        void Channel::CtrlChange(int32 tag, ParamValue value) {
            switch (tag & PARAM_TAG_MASK) {
            case PARAM_TAG_MASTER_VOLUME:
                MasterVolume = value;
                break;
            case PARAM_TAG_CHANNEL_VOL:
                Vol = value;
                break;
            case PARAM_TAG_CHANNEL_EXP:
                Exp = value;
                break;
            case PARAM_TAG_CHANNEL_PAN:
                Pan = value;
                mPanL = cos(1.57 * value);
                mPanR = sin(1.57 * value);
                break;
            }
        }

        void Channel::Step(ProcessData& data) {
            Sample32* outL = data.outputs[0].channelBuffers32[0];
            Sample32* outR = data.outputs[0].channelBuffers32[1];

            for (int32 i = 0; i < data.numSamples; i++) {
                outL[i] += pWaveL[WriteIndex] * mCurAmp;
                outR[i] += pWaveR[WriteIndex] * mCurAmp;

                mCurAmp += (Vol * Exp * MasterVolume - mCurAmp) * 0.01;
                CurPanL += (mPanL - CurPanL) * 0.01;
                CurPanR += (mPanR - CurPanR) * 0.01;

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
