#include "sampler.h"
#include "channel.h"

namespace Steinberg {
    namespace Vst {
        Sampler *Sampler::List[SAMPLER_COUNT] = { NULL };

        void Sampler::Step(ProcessData& data) {
            if (State < SAMPLER_STATE::PRESS) {
                return;
            }

            Channel *pCh = Channel::List[ChannelNumber];
            double* pWaveL = pCh->pWaveL;
            double* pWaveR = pCh->pWaveR;
            int writeIndex = pCh->WriteIndex;

            for (int32 i = 0; i < data.numSamples; i++) {
                /* generate envelope */
                switch (State) {
                case SAMPLER_STATE::PRESS:
                    if (Time < pCh->AmpAttack) {
                        CurAmp += (1.0 - CurAmp) * Delta / pCh->AmpAttack;
                    } else {
                        CurAmp += (pCh->AmpSustain - CurAmp) * Delta / pCh->AmpDecay;
                    }
                    break;
                case SAMPLER_STATE::HOLD:
                    CurAmp -= CurAmp * Delta;
                    break;
                case SAMPLER_STATE::RELEASE:
                    CurAmp -= CurAmp * Delta / pCh->AmpRelease;
                    break;
                case SAMPLER_STATE::PURGE:
                default:
                    CurAmp -= CurAmp * Delta / 0.001;
                    break;
                }
                
                if (CurAmp < 0.0005) {
                    State = SAMPLER_STATE::FREE;
                    break;
                }

                /* generate wave */
                re -= im * 6.28 * Pitch * Delta;
                im += re * 6.28 * Pitch * Delta;

                /* output */
                pWaveL[writeIndex] += re * CurAmp * Gain;
                pWaveR[writeIndex] += im * CurAmp * Gain;

                /* update time */
                Time += Delta;
                writeIndex++;
                if (BUFF_SIZE <= writeIndex) {
                    writeIndex -= BUFF_SIZE;
                }
            }
        }
    }
}
