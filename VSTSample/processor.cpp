// 自作VST用のインクルードファイル
#include "myvst3define.h"
#include "myvst3fuid.h"
#include "processor.h"
#include "controller.h"
#include "channel.h"
#include "sampler.h"

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {

		// =================================================================================
		// コンストラクタ
		// =================================================================================
		MyVSTProcessor::MyVSTProcessor() {
			// コントローラーのFUIDを設定する
			setControllerClass(ControllerUID);
		}

		// ===================================================================================
		// クラスを初期化する関数
		// ===================================================================================
		tresult PLUGIN_API MyVSTProcessor::initialize(FUnknown* context) {
			// まず継承元クラスの初期化を実施
			tresult result = AudioEffect::initialize(context);
			if (result == kResultTrue) {
				// 入力と出力を設定
				addAudioInput(STR16("AudioInput"), SpeakerArr::kStereo);
				addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);

				// 入力のEventBusを追加する
				addEventInput(STR16("Event Input"), 1);

				// 以下固有の初期化を実施。
				for (int i = 0; i < CHANNEL_COUNT; i++) {
					Channel::List[i] = new Channel();
				}
				for (int i = 0; i < SAMPLER_COUNT; i++) {
					Sampler::List[i] = new Sampler();
				}
			}

			// 初期化が成功すればkResultTrueを返す。
			return result;
		}

		// ===================================================================================
		// クラスを解放する関数
		// ===================================================================================
		tresult PLUGIN_API MyVSTProcessor::terminate() {
			for (int i = 0; i < CHANNEL_COUNT; i++) {
				delete Channel::List[i];
			}
			for (int i = 0; i < SAMPLER_COUNT; i++) {
				delete Sampler::List[i];
			}
			return kResultTrue;
		}

		tresult PLUGIN_API MyVSTProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) {
			// inputとoutputのバスが1つずつで、かつinputとoutputの構成がステレオの場合
			if (numIns == 1 && numOuts == 1 && inputs[0] == SpeakerArr::kStereo && outputs[0] == SpeakerArr::kStereo) {
				return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
			}

			// 対応していないバス構成の場合、kResultFalseを返す。
			return kResultFalse;
		}

		tresult PLUGIN_API MyVSTProcessor::setState(IBStream* state) {
			// 現在のProcessorクラスの状態を読込
			// マルチプラットフォーム対応にする場合はエンディアンに注意

			// 保存されているデータを読み込む
			// 保存されているデータが複数ある場合はstate->readを繰り返す
			tresult res;
			res = state->read(&masterVolume, sizeof(ParamValue));
			if (res != kResultOk) {
				// 読込に失敗した場合はkResultFalseを返す。
				return kResultFalse;
			}

			// 関数の処理に問題がなければkResultOkを返す
			return kResultOk;
		}

		tresult PLUGIN_API MyVSTProcessor::getState(IBStream* state) {
			// 現在のProcessorクラスの状態を保存
			// マルチプラットフォーム対応にする場合はエンディアンに注意

			// データを保存する
			// 保存したいデータが複数ある場合はstate->writeを繰り返す。
			state->write(&masterVolume, sizeof(ParamValue));

			// 関数の処理に問題がなければkResultOkを返す
			return kResultOk;
		}

		// ===================================================================================
		// 音声信号を処理する関数
		// ===================================================================================
		tresult PLUGIN_API MyVSTProcessor::process(ProcessData& data) {
			// パラメーター変更の処理
			// 与えられたパラメーターがあるとき、dataのinputParameterChangesに
			// IParameterChangesクラスへのポインタのアドレスが入る
			if (data.inputParameterChanges != NULL) {
				// 与えられたパラメーターの数を取得
				int32 paramChangeCount = data.inputParameterChanges->getParameterCount();

				// 与えられたパラメーター分、処理を繰り返す。
				for (int32 i = 0; i < paramChangeCount; i++) {
					// パラメーター変更のキューを取得
					// (処理するサンプル内に複数のパラメーター変更情報がある可能性があるため、
					// キューという形になっている。)
					IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);
					if (queue != NULL) {
						// どのパラメーターが変更されたか知るため、パラメーターtagを取得
						int32 tag = queue->getParameterId();

						// 変更された回数を取得
						int32 valueChangeCount = queue->getPointCount();
						ParamValue value;
						int32 sampleOffset;

						// 最後に変更された値を取得
						if (queue->getPoint(valueChangeCount - 1, sampleOffset, value) == kResultTrue) {
							// tagに応じた処理を実施
							Channel *pCh;
							switch (tag & PARAM_TAG_MASK) {
							case PARAM_TAG_MASTER_VOLUME:
								// volumeはメンバー変数としてあらかじめ定義・初期化しておく。
								masterVolume = value;
								break;
							case PARAM_TAG_CHANNEL_VOL:
								Channel::List[tag & 0xFF]->Vol = value;
								break;
							case PARAM_TAG_CHANNEL_EXP:
								Channel::List[tag & 0xFF]->Exp = value;
								break;
							case PARAM_TAG_CHANNEL_PAN:
								pCh = Channel::List[tag & 0xFF];
								pCh->Pan = value;
								pCh->PanL = cos(1.57 * value);
								pCh->PanR = sin(1.57 * value);
								break;
							}
						}
					}
				}
			}

			// イベントの処理
			// 与えられたイベントがあるときdataのinputEventsに
			// IEventListクラスへのポインタのアドレスが入る
			IEventList* eventList = data.inputEvents;
			if (eventList != NULL) {
				// イベントの数を取得する。
				int32 numEvent = eventList->getEventCount();
				for (int32 i = 0; i < numEvent; i++) {
					// i番目のイベントデータを取得する
					Event event;
					if (eventList->getEvent(i, event) == kResultOk) {
						int16 channel;

						// イベントデータのタイプごとに振り分け
						switch (event.type) {
						case Event::kNoteOnEvent: // ノートオンイベントの場合
							channel = event.noteOn.channel;
							onNoteOn(channel, event.noteOn.pitch, event.noteOn.velocity);
							break;

						case Event::kNoteOffEvent: // ノートオフイベントの場合
							channel = event.noteOff.channel;
							onNoteOff(channel, event.noteOff.pitch, event.noteOff.velocity);
							break;
						}
					}
				}
			}

			// 入力・出力バッファのポインタをわかりやすい変数に格納
			// inputs[]、outputs[]はAudioBusの数だけある(addAudioInput()、addAudioOutput()で追加した分だけ)
			// 今回はAudioBusは1つだけなので 0 のみとなる
			// channelBuffers32は32bit浮動小数点型のバッファで音声信号のチャンネル数分ある
			// モノラル(kMono)なら 0 のみで、ステレオ(kStereo)なら 0(Left) と 1(Right) となる
			//Sample32* inL = data.inputs[0].channelBuffers32[0];
			//Sample32* inR = data.inputs[0].channelBuffers32[1];
			Sample32* outL = data.outputs[0].channelBuffers32[0];
			Sample32* outR = data.outputs[0].channelBuffers32[1];
			memset(outL, 0, sizeof(Sample32) * data.numSamples);
			memset(outR, 0, sizeof(Sample32) * data.numSamples);

			for (int s = 0; s < SAMPLER_COUNT; s++) {
				Sampler::List[s]->Step(data);
			}
			for (int c = 0; c < CHANNEL_COUNT; c++) {
				Channel::List[c]->Step(data);
			}

			// 問題なければkResultTrueを返す(おそらく必ずkResultTrueを返す)
			return kResultTrue;
		}

		void MyVSTProcessor::onNoteOn(int channel, int note, float velocity) {
			// MIDIノートオンイベントの処理を行う
			if (0 == velocity) {
				onNoteOff(channel, note, velocity);
				return;
			}
			for (int i = 0; i < SAMPLER_COUNT; i++) {
				auto pSmpl = Sampler::List[i];
				if (pSmpl->ChannelNumber == channel && pSmpl->NoteNumber == note && SAMPLER_STATE::PRESS <= pSmpl->State) {
					pSmpl->State = SAMPLER_STATE::PURGE;
				}
			}
			for (int i = 0; i < SAMPLER_COUNT; i++) {
				auto pSmpl = Sampler::List[i];
				if (pSmpl->State == SAMPLER_STATE::FREE) {
					pSmpl->State = SAMPLER_STATE::RESERVED;
					pSmpl->ChannelNumber = channel;
					pSmpl->NoteNumber = note;
					// 押されたノートから、音程を計算
					// ノートNo.69が440Hzになる。これを基準に計算する。
					// 計算式の詳細説明については割愛
					pSmpl->Pitch = 440.0 * pow(2.0f, (note - 69) / 12.0);
					pSmpl->Delta = 1.0 / processSetup.sampleRate;
					pSmpl->Gain = velocity;
					pSmpl->CurAmp = 0.001;
					pSmpl->Time = 0.0;
					pSmpl->State = SAMPLER_STATE::PRESS;
					break;
				}
			}
		}

		void MyVSTProcessor::onNoteOff(int channel, int note, float velocity) {
			// MIDIノートオフイベントの処理を行う
			for (int i = 0; i < SAMPLER_COUNT; i++) {
				auto pSmpl = Sampler::List[i];
				if (pSmpl->ChannelNumber == channel && pSmpl->NoteNumber == note) {
					pSmpl->State = SAMPLER_STATE::RELEASE;
				}
			}
		}
	}
}
