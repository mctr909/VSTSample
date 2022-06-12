#include "sampler.h"
#include "channel.h"

namespace Steinberg {
    namespace Vst {
        ProcessSetup* Sampler::Setup = NULL;
        Sampler *Sampler::List[SAMPLER_COUNT] = { NULL };

        void Sampler::NoteOn(int channel, int noteNumber, float velocity) {
            State = SAMPLER_STATE::RESERVED;
            ChannelNumber = channel;
            NoteNumber = noteNumber;
            // 押されたノートから、音程を計算
            // ノートNo.69が440Hzになる。これを基準に計算する。
            // 計算式の詳細説明については割愛
            mPitch = 440.0 * pow(2.0f, (noteNumber - 69) / 12.0);
            mGain = velocity;
            mCurAmp = 0.001;
            mTime = 0.0;
            State = SAMPLER_STATE::PRESS;
        }

        void Sampler::Step(ProcessData& data) {
            if (State < SAMPLER_STATE::PRESS) {
                return;
            }

            Channel *pCh = Channel::List[ChannelNumber];
            double* pWaveL = pCh->pWaveL;
            double* pWaveR = pCh->pWaveR;
            int writeIndex = pCh->WriteIndex;
            double delta = 1.0 / Setup->sampleRate;

            for (int32 i = 0; i < data.numSamples; i++) {
                /* generate envelope */
                switch (State) {
                case SAMPLER_STATE::PRESS:
                    if (mTime < pCh->AmpAttack) {
                        mCurAmp += (1.0 - mCurAmp) * delta / pCh->AmpAttack;
                    } else {
                        mCurAmp += (pCh->AmpSustain - mCurAmp) * delta / pCh->AmpDecay;
                    }
                    break;
                case SAMPLER_STATE::HOLD:
                    mCurAmp -= mCurAmp * delta;
                    break;
                case SAMPLER_STATE::RELEASE:
                    mCurAmp -= mCurAmp * delta / pCh->AmpRelease;
                    break;
                case SAMPLER_STATE::PURGE:
                default:
                    mCurAmp -= mCurAmp * delta / 0.001;
                    break;
                }
                
                if (mCurAmp < 0.0005) {
                    State = SAMPLER_STATE::FREE;
                    break;
                }

                /* generate wave */
                re -= im * 6.28 * mPitch * delta;
                im += re * 6.28 * mPitch * delta;

                /* output */
                pWaveL[writeIndex] += re * mCurAmp * mGain;
                pWaveR[writeIndex] += im * mCurAmp * mGain;

                /* update time */
                mTime += delta;
                writeIndex++;
                if (BUFF_SIZE <= writeIndex) {
                    writeIndex -= BUFF_SIZE;
                }
            }
        }
    }
}
