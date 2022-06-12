// ����VST�p�̃C���N���[�h�t�@�C��
#include "myvst3define.h"
#include "myvst3fuid.h"
#include "processor.h"
#include "controller.h"
#include "channel.h"
#include "sampler.h"

// VST3�쐬�ɕK�v�Ȃ̖��O��Ԃ��g�p
namespace Steinberg {
	namespace Vst {

		// =================================================================================
		// �R���X�g���N�^
		// =================================================================================
		MyVSTProcessor::MyVSTProcessor() {
			// �R���g���[���[��FUID��ݒ肷��
			setControllerClass(ControllerUID);
		}

		// ===================================================================================
		// �N���X������������֐�
		// ===================================================================================
		tresult PLUGIN_API MyVSTProcessor::initialize(FUnknown* context) {
			// �܂��p�����N���X�̏����������{
			tresult result = AudioEffect::initialize(context);
			if (result == kResultTrue) {
				// ���͂Əo�͂�ݒ�
				addAudioInput(STR16("AudioInput"), SpeakerArr::kStereo);
				addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);

				// ���͂�EventBus��ǉ�����
				addEventInput(STR16("Event Input"), 1);

				// �ȉ��ŗL�̏����������{�B
				for (int i = 0; i < CHANNEL_COUNT; i++) {
					Channel::List[i] = new Channel();
				}
				for (int i = 0; i < SAMPLER_COUNT; i++) {
					Sampler::List[i] = new Sampler();
				}
			}

			// �����������������kResultTrue��Ԃ��B
			return result;
		}

		// ===================================================================================
		// �N���X���������֐�
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
			// input��output�̃o�X��1���ŁA����input��output�̍\�����X�e���I�̏ꍇ
			if (numIns == 1 && numOuts == 1 && inputs[0] == SpeakerArr::kStereo && outputs[0] == SpeakerArr::kStereo) {
				return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
			}

			// �Ή����Ă��Ȃ��o�X�\���̏ꍇ�AkResultFalse��Ԃ��B
			return kResultFalse;
		}

		tresult PLUGIN_API MyVSTProcessor::setState(IBStream* state) {
			// ���݂�Processor�N���X�̏�Ԃ�Ǎ�
			// �}���`�v���b�g�t�H�[���Ή��ɂ���ꍇ�̓G���f�B�A���ɒ���

			// �ۑ�����Ă���f�[�^��ǂݍ���
			// �ۑ�����Ă���f�[�^����������ꍇ��state->read���J��Ԃ�
			tresult res;
			res = state->read(&masterVolume, sizeof(ParamValue));
			if (res != kResultOk) {
				// �Ǎ��Ɏ��s�����ꍇ��kResultFalse��Ԃ��B
				return kResultFalse;
			}

			// �֐��̏����ɖ�肪�Ȃ����kResultOk��Ԃ�
			return kResultOk;
		}

		tresult PLUGIN_API MyVSTProcessor::getState(IBStream* state) {
			// ���݂�Processor�N���X�̏�Ԃ�ۑ�
			// �}���`�v���b�g�t�H�[���Ή��ɂ���ꍇ�̓G���f�B�A���ɒ���

			// �f�[�^��ۑ�����
			// �ۑ��������f�[�^����������ꍇ��state->write���J��Ԃ��B
			state->write(&masterVolume, sizeof(ParamValue));

			// �֐��̏����ɖ�肪�Ȃ����kResultOk��Ԃ�
			return kResultOk;
		}

		// ===================================================================================
		// �����M������������֐�
		// ===================================================================================
		tresult PLUGIN_API MyVSTProcessor::process(ProcessData& data) {
			// �p�����[�^�[�ύX�̏���
			// �^����ꂽ�p�����[�^�[������Ƃ��Adata��inputParameterChanges��
			// IParameterChanges�N���X�ւ̃|�C���^�̃A�h���X������
			if (data.inputParameterChanges != NULL) {
				// �^����ꂽ�p�����[�^�[�̐����擾
				int32 paramChangeCount = data.inputParameterChanges->getParameterCount();

				// �^����ꂽ�p�����[�^�[���A�������J��Ԃ��B
				for (int32 i = 0; i < paramChangeCount; i++) {
					// �p�����[�^�[�ύX�̃L���[���擾
					// (��������T���v�����ɕ����̃p�����[�^�[�ύX��񂪂���\�������邽�߁A
					// �L���[�Ƃ����`�ɂȂ��Ă���B)
					IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);
					if (queue != NULL) {
						// �ǂ̃p�����[�^�[���ύX���ꂽ���m�邽�߁A�p�����[�^�[tag���擾
						int32 tag = queue->getParameterId();

						// �ύX���ꂽ�񐔂��擾
						int32 valueChangeCount = queue->getPointCount();
						ParamValue value;
						int32 sampleOffset;

						// �Ō�ɕύX���ꂽ�l���擾
						if (queue->getPoint(valueChangeCount - 1, sampleOffset, value) == kResultTrue) {
							// tag�ɉ��������������{
							Channel *pCh;
							switch (tag & PARAM_TAG_MASK) {
							case PARAM_TAG_MASTER_VOLUME:
								// volume�̓����o�[�ϐ��Ƃ��Ă��炩���ߒ�`�E���������Ă����B
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

			// �C�x���g�̏���
			// �^����ꂽ�C�x���g������Ƃ�data��inputEvents��
			// IEventList�N���X�ւ̃|�C���^�̃A�h���X������
			IEventList* eventList = data.inputEvents;
			if (eventList != NULL) {
				// �C�x���g�̐����擾����B
				int32 numEvent = eventList->getEventCount();
				for (int32 i = 0; i < numEvent; i++) {
					// i�Ԗڂ̃C�x���g�f�[�^���擾����
					Event event;
					if (eventList->getEvent(i, event) == kResultOk) {
						int16 channel;

						// �C�x���g�f�[�^�̃^�C�v���ƂɐU�蕪��
						switch (event.type) {
						case Event::kNoteOnEvent: // �m�[�g�I���C�x���g�̏ꍇ
							channel = event.noteOn.channel;
							onNoteOn(channel, event.noteOn.pitch, event.noteOn.velocity);
							break;

						case Event::kNoteOffEvent: // �m�[�g�I�t�C�x���g�̏ꍇ
							channel = event.noteOff.channel;
							onNoteOff(channel, event.noteOff.pitch, event.noteOff.velocity);
							break;
						}
					}
				}
			}

			// ���́E�o�̓o�b�t�@�̃|�C���^���킩��₷���ϐ��Ɋi�[
			// inputs[]�Aoutputs[]��AudioBus�̐���������(addAudioInput()�AaddAudioOutput()�Œǉ�����������)
			// �����AudioBus��1�����Ȃ̂� 0 �݂̂ƂȂ�
			// channelBuffers32��32bit���������_�^�̃o�b�t�@�ŉ����M���̃`�����l����������
			// ���m����(kMono)�Ȃ� 0 �݂̂ŁA�X�e���I(kStereo)�Ȃ� 0(Left) �� 1(Right) �ƂȂ�
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

			// ���Ȃ����kResultTrue��Ԃ�(�����炭�K��kResultTrue��Ԃ�)
			return kResultTrue;
		}

		void MyVSTProcessor::onNoteOn(int channel, int note, float velocity) {
			// MIDI�m�[�g�I���C�x���g�̏������s��
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
					// �����ꂽ�m�[�g����A�������v�Z
					// �m�[�gNo.69��440Hz�ɂȂ�B�������Ɍv�Z����B
					// �v�Z���̏ڍא����ɂ��Ă͊���
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
			// MIDI�m�[�g�I�t�C�x���g�̏������s��
			for (int i = 0; i < SAMPLER_COUNT; i++) {
				auto pSmpl = Sampler::List[i];
				if (pSmpl->ChannelNumber == channel && pSmpl->NoteNumber == note) {
					pSmpl->State = SAMPLER_STATE::RELEASE;
				}
			}
		}
	}
}
