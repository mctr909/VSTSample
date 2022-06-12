#ifndef __HEADER_CHANNEL__
#define __HEADER_CHANNEL__

// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vstaudioeffect.h"

// 自作VST用のインクルードファイル
#include "myvst3define.h"

namespace Steinberg {
    namespace Vst {
        class Channel {
        public:
            static Channel* List[CHANNEL_COUNT];

        private:
            double mCurAmp = 0.0;
            double mCurPanL = 0.0;
            double mCurPanR = 0.0;

        public:
            ParamValue Vol = 0.0;
            ParamValue Exp = 0.0;
            ParamValue Pan = 0.0;
            double PanL = 0.0;
            double PanR = 0.0;
            double Pitch = 1.0;
            int BendRange = 2;
            int WriteIndex = 0;
            double* pWaveL = NULL;
            double* pWaveR = NULL;

        public:
            Channel();
            ~Channel();
            void Reset();
            void Step(ProcessData& data);
        };
    }
}

#endif // __HEADER_CHANNEL__
