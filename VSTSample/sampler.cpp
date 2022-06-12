#include "sampler.h"
#include "channel.h"

namespace Steinberg {
    namespace Vst {
        Sampler *Sampler::List[SAMPLER_COUNT] = { NULL };

        void Sampler::Step(ProcessData& data) {
            if (state < SAMPLER_STATE::ACTIVE) {
                return;
            }

            Channel *pCh = Channel::List[channelNumber];
            double* pWaveL = pCh->pWaveL;
            double* pWaveR = pCh->pWaveR;
            int writeIndex = pCh->WriteIndex;

            for (int32 i = 0; i < data.numSamples; i++) {
                re -= im * delta * 6.28;
                im += re * delta * 6.28;

                pWaveL[writeIndex] += re * gain;
                pWaveR[writeIndex] += im * gain;
                writeIndex++;
                if (BUFF_SIZE <= writeIndex) {
                    writeIndex -= BUFF_SIZE;
                }
            }
        }
    }
}
