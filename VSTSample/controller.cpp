// 自作VST用のインクルードファイル
#include "myvst3define.h"
#include "myvst3fuid.h"
#include "controller.h"

// VST3作成に必要な名前空間を使用
namespace Steinberg {
	namespace Vst {

		// クラスを初期化する関数(必須)
		tresult PLUGIN_API MyVSTController::initialize(FUnknown* context)
		{
			// まず継承元クラスの初期化を実施
			tresult result = EditController::initialize(context);
			if (result == kResultTrue)
			{
				// パラメーターを追加
				parameters.addParameter(STR16("param1"), STR16("..."), 0, 1, ParameterInfo::kCanAutomate, PARAM_TAG_MASTER_VOLUME);

				// 以下固有の初期化を実施。

				// 今回は何もしない
			}

			// 初期化が成功すればkResultTrueを返す。
			result = kResultTrue;
			return result;
		}

		tresult PLUGIN_API MyVSTController::setComponentState(IBStream* state)
		{
			// 現在のProcessorクラスの状態を読込
			// マルチプラットフォーム対応にする場合はエンディアンに注意
			tresult res;

			// 保存されているデータを読み込む
			// 保存されているデータが複数ある場合はstate->readを繰り返す
			ParamValue value;
			res = state->read(&value, sizeof(ParamValue));
			if (res != kResultOk)
			{
				// 読込に失敗した場合はkResultFalseを返す。
				return kResultFalse;
			}
			// 読み込まれたデータをパラメーターに反映する
			// 反映するデータは0.0～1.0の範囲にしておく
			value = plainParamToNormalized(PARAM_TAG_MASTER_VOLUME, value);
			setParamNormalized(PARAM_TAG_MASTER_VOLUME, value);

			// 関数の処理に問題がなければkResultOkを返す
			return kResultOk;
		}

		tresult PLUGIN_API MyVSTController::getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id)
		{

			// midiControllerNumberに関係付けたいパラメータータグ(ID)をidに代入することで
			// MIDIコントロールチェンジとパラメーターを関連付ける
			// イベントバスやMIDIチャンネルに応じて関連付けを変更したい場合は
			// busIndexやchannelを使用して条件分けする
			switch (midiControllerNumber)
			{
			case kCtrlVolume:
				return kResultTrue;
			case kCtrlExpression:
				return kResultTrue;
			}

			// 関連付けるパラメーターがない場合、kResultFalseを返す
			return kResultFalse;
		}
	}
}
