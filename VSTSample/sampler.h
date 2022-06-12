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
            static Sampler* List[SAMPLER_COUNT];

        public:
            SAMPLER_STATE State = SAMPLER_STATE::FREE;
            int ChannelNumber = 0;
            int NoteNumber = 0;
            double Pitch = 1.0;
            double Delta = 1.0;
            double Gain = 1.0;
            double CurAmp = 0.0;
            double Time = 0.0;

            double re = 0.0;
            double im = 0.2;

        public:
            void Step(ProcessData& data);
        };
    }
}

#endif // __HEADER_SAMPLER__
