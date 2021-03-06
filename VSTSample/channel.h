#ifndef __HEADER_CHANNEL__
#define __HEADER_CHANNEL__

// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vstaudioeffect.h"

// 自作VST用のインクルードファイル
#include "myvst3define.h"
#include "dls.h"

namespace Steinberg {
    namespace Vst {
        class Channel {
        public:
            static ProcessSetup* Setup;
            static Channel* List[CHANNEL_COUNT];
            static ParamValue MasterVolume;

        private:
            double mCurAmp = 0.0;
            double mPanL = 0.0;
            double mPanR = 0.0;

        public:
            uint8 Number = 0;
            ParamValue Vol = 0.0;
            ParamValue Exp = 0.0;
            ParamValue Pan = 0.0;
            ParamValue Pitch = 1.0;
            ParamValue BendRange = 2;

            int WriteIndex = 0;
            double* pWaveL = NULL;
            double* pWaveR = NULL;
            double CurPanL = 0.0;
            double CurPanR = 0.0;

            INS_* pInst = NULL;

        public:
            Channel(uint8 number);
            ~Channel();
            void Reset();
            void CtrlChange(int32 tag, ParamValue value);
            void Step(ProcessData& data);
        };
    }
}

#endif // __HEADER_CHANNEL__
