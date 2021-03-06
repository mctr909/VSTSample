// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstevents.h" // イベントバス利用時に必要

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {

		// ===================================================================================
		// 音声信号を処理するProcessorクラス
		// ===================================================================================
		class MyVSTProcessor : public AudioEffect {
		public:
			// コンストラクタ
			MyVSTProcessor();

			// クラスを初期化する関数(必須)
			tresult PLUGIN_API initialize(FUnknown* context);

			// クラスを解放する関数
			tresult PLUGIN_API terminate();

			// バス構成を設定する関数。
			tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts);

			// Processorクラスの状態の読込と保存する関数
			tresult PLUGIN_API setState(IBStream* state);
			tresult PLUGIN_API getState(IBStream* state);

			// 音声信号を処理する関数(必須)
			tresult PLUGIN_API process(ProcessData& data);

			// 自作VST Processorクラスのインスタンスを作成するための関数(必須)
			static FUnknown* createInstance(void*) { return (IAudioProcessor*)new MyVSTProcessor(); }

			void onNoteOn(int channel, int note, float velocity);
			void onNoteOff(int channel, int note, float velocity);
		};

	}
}
