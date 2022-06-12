#ifndef __HEADER_CHANNEL__
#define __HEADER_CHANNEL__

// VST3 SDK�̃C���N���[�h�t�@�C��
#include "public.sdk/source/vst/vstaudioeffect.h"

// ����VST�p�̃C���N���[�h�t�@�C��
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

            ParamValue Pitch = 1.0;
            ParamValue BendRange = 2;

            ParamValue AmpAttack = 0.001;
            ParamValue AmpDecay = 0.05;
            ParamValue AmpSustain = 0.33;
            ParamValue AmpRelease = 0.001;

            double PanL = 0.0;
            double PanR = 0.0;
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
