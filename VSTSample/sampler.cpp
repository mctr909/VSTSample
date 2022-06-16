#include "sampler.h"
#include "channel.h"

namespace Steinberg {
    namespace Vst {
        ProcessSetup* Sampler::Setup = NULL;
        Sampler *Sampler::List[SAMPLER_COUNT] = { NULL };

        void Sampler::NoteOn(int channel, int noteNumber, float velocity) {
            State = SAMPLER_STATE::RESERVED;
            Channel* pCh = Channel::List[channel];
            ChannelNumber = channel;
            NoteNumber = noteNumber;
            auto rgns = pCh->pInst->GetRegions(noteNumber, (uint8)(127 * velocity));
            if (0 == rgns.size()) {
                State = SAMPLER_STATE::FREE;
                return;
            }

            auto rgn = rgns[0];
            auto wave = DLS::Instance->GetWave(rgn);
            mpWave = wave->pWave;
            if (wave->pLoop) {
                mLoopBegin = wave->pLoop->start;
                mLoopLength = wave->pLoop->length;
                mLoopEnable = true;
            } else {
                mLoopBegin = 0;
                mLoopLength = wave->Size / 2;
                mLoopEnable = false;
            }

            AmpAttack = 0.001;
            AmpDecay = 1.0;
            AmpRelease = 0.001;
            AmpSustain = 0.0;
            if (NULL == rgn->pArtList) {
                auto count = pCh->pInst->pArtList->Count;
                auto pConn = pCh->pInst->pArtList->pConn;
                for (uint32 i = 0; i < count; i++) {
                    switch (pConn->destination) {
                    case E_DLS_DST::EG1_ATTACK_TIME:
                        AmpAttack = pConn->getValue(); break;
                    case E_DLS_DST::EG1_DECAY_TIME:
                        AmpDecay = pConn->getValue(); break;
                    case E_DLS_DST::EG1_RELEASE_TIME:
                        AmpRelease = pConn->getValue(); break;
                    case E_DLS_DST::EG1_SUSTAIN_LEVEL:
                        AmpSustain = pConn->getValue(); break;
                    default:
                        break;
                    }
                }
            } else {
                auto count = rgn->pArtList->Count;
                auto pConn = rgn->pArtList->pConn;
                for (uint32 i = 0; i < count; i++) {
                    switch (pConn->destination) {
                    case E_DLS_DST::EG1_ATTACK_TIME:
                        AmpAttack = pConn->getValue(); break;
                    case E_DLS_DST::EG1_DECAY_TIME:
                        AmpDecay = pConn->getValue(); break;
                    case E_DLS_DST::EG1_RELEASE_TIME:
                        AmpRelease = pConn->getValue(); break;
                    case E_DLS_DST::EG1_SUSTAIN_LEVEL:
                        AmpSustain = pConn->getValue(); break;
                    default:
                        break;
                    }
                }
            }

            mGain = velocity;
            auto note_diff = noteNumber - wave->pWsmp->unityNote;
            mPitch = pow(2.0f, note_diff / 12.0);
            mPitch *= wave->pWsmp->getFineTune();
            mPitch *= wave->pFmt->sampleRate / OVER_SAMPLING;
            mCurAmp = 0.001;
            mTime = 0.0;
            mIndex = 0.0;
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
                    if (mTime < AmpAttack) {
                        mCurAmp += (1.0 - mCurAmp) * delta / AmpAttack;
                    } else {
                        mCurAmp += (AmpSustain - mCurAmp) * delta / AmpDecay;
                    }
                    break;
                case SAMPLER_STATE::HOLD:
                    mCurAmp -= mCurAmp * delta;
                    break;
                case SAMPLER_STATE::RELEASE:
                    mCurAmp -= mCurAmp * delta / AmpRelease;
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
                double smoothedWave = 0.0;
                for (int o = 0; o < OVER_SAMPLING; o++) {
                    auto iIdx = (int32)mIndex;
                    auto di = mIndex - iIdx;
                    smoothedWave += mpWave[iIdx - 1] * (1.0 - di) + mpWave[iIdx] * di;
                    mIndex += mPitch * delta;
                    if ((mLoopBegin + mLoopLength) < mIndex) {
                        if (mLoopEnable) {
                            mIndex -= mLoopLength;
                        } else {
                            State = SAMPLER_STATE::FREE;
                            break;
                        }
                    }
                }
                smoothedWave *= mCurAmp * mGain / 32768.0 / OVER_SAMPLING;

                /* output */
                pWaveL[writeIndex] += smoothedWave * pCh->CurPanL;
                pWaveR[writeIndex] += smoothedWave * pCh->CurPanR;

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
