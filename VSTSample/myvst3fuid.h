// VST3 SDKのインクルードファイル
#include "pluginterfaces\base\funknown.h"

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
    namespace Vst {
        // FUIDの生成
        static const FUID ProcessorUID(0xBBCFA27E, 0x451F45DD, 0xBEBF0625, 0xA4A9E2D5);
        static const FUID ControllerUID(0x36874AB3, 0x7E69470F, 0xA72B8A88, 0x43E1D8A4);
    }
}
