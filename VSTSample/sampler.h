#ifndef __HEADER_SAMPLER__
#define __HEADER_SAMPLER__

// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vstaudioeffect.h"

// 自作VST用のインクルードファイル
#include "myvst3define.h"

namespace Steinberg {
    namespace Vst {
        enum struct SAMPLER_STATE {
            FREE,
            RESERVED,
            PRESS,
            HOLD,
            RELEASE,
            PURGE
        };

        class Sampler {
        public:
            static ProcessSetup* Setup;
            static Sampler* List[SAMPLER_COUNT];

        private:
            double mPitch = 1.0;
            double mGain = 1.0;
            double mCurAmp = 0.0;
            double mTime = 0.0;

            double re = 0.0;
            double im = 0.2;

        public:
            SAMPLER_STATE State = SAMPLER_STATE::FREE;
            int ChannelNumber = 0;
            int NoteNumber = 0;

        public:
            void NoteOn(int channel, int noteNumber, float velocity);
            void Step(ProcessData& data);
        };
    }
}

#endif // __HEADER_SAMPLER__
