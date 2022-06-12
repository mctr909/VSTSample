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
            ACTIVE,
            HOLD,
            RELEASE,
            PURGE
        };

        class Sampler {
        public:
            static Sampler* List[SAMPLER_COUNT];

        public:
            SAMPLER_STATE state = SAMPLER_STATE::FREE;
            int channelNumber = 0;
            int noteNumber = 0;
            double delta = 1.0;
            double gain = 1.0;
            double re = 0.0;
            double im = 0.01;

        public:
            void Step(ProcessData& data);
        };
    }
}

#endif // __HEADER_SAMPLER__
