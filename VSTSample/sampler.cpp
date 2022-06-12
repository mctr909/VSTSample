#include "sampler.h"
#include "channel.h"

namespace Steinberg {
    namespace Vst {
        Sampler *Sampler::List[SAMPLER_COUNT] = { NULL };

        void Sampler::Step(ProcessData& data) {
            if (state < SAMPLER_STATE::PRESS) {
                return;
            }

            Channel *pCh = Channel::List[channelNumber];
            double* pWaveL = pCh->pWaveL;
            double* pWaveR = pCh->pWaveR;
            int writeIndex = pCh->WriteIndex;

            for (int32 i = 0; i < data.numSamples; i++) {
                /* generate envelope */
                switch (state) {
                case SAMPLER_STATE::PRESS:
                    if (time < 0.002) {
                        curAmp += (1 - curAmp) * 500 * delta;
                    } else {
                        curAmp += (0.33 - curAmp) * 20 * delta;
                    }
                    break;
                case SAMPLER_STATE::HOLD:
                    curAmp -= curAmp * delta;
                    break;
                case SAMPLER_STATE::RELEASE:
                    curAmp -= curAmp * 500 * delta;
                    break;
                case SAMPLER_STATE::PURGE:
                    curAmp -= curAmp * 500 * delta;
                    break;
                default:
                    break;
                }

                if (curAmp < 0.0005) {
                    state = SAMPLER_STATE::FREE;
                    break;
                }

                /* generate wave */
                re -= im * 6.28 * pitch * delta;
                im += re * 6.28 * pitch * delta;

                /* output */
                pWaveL[writeIndex] += re * curAmp * gain;
                pWaveR[writeIndex] += im * curAmp * gain;

                /* update time */
                time += delta;
                writeIndex++;
                if (BUFF_SIZE <= writeIndex) {
                    writeIndex -= BUFF_SIZE;
                }
            }
        }
    }
}
