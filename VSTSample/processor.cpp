// ����VST�p�̃C���N���[�h�t�@�C��
#include "myvst3define.h"
#include "myvst3fuid.h"
#include "processor.h"
#include "controller.h"

// VST3�쐬�ɕK�v�Ȃ̖��O��Ԃ��g�p
namespace Steinberg {
	namespace Vst {

		// =================================================================================
		// �R���X�g���N�^
		// =================================================================================
		MyVSTProcessor::MyVSTProcessor()
		{
			// �R���g���[���[��FUID��ݒ肷��
			setControllerClass(ControllerUID);
		}

		// ===================================================================================
		// �N���X������������֐�
		// ===================================================================================
		tresult PLUGIN_API MyVSTProcessor::initialize(FUnknown* context)
		{
			// �܂��p�����N���X�̏����������{
			tresult result = AudioEffect::initialize(context);
			if (result == kResultTrue)
			{
				// ���͂Əo�͂�ݒ�
				addAudioInput(STR16("AudioInput"), SpeakerArr::kStereo);
				addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);

				// ���͂�EventBus��ǉ�����
				addEventInput(STR16("Event Input"), 1);

				// �ȉ��ŗL�̏����������{�B

				// ����͉������Ȃ�
			}


			// �����������������kResultTrue��Ԃ��B
			return result;
		}

		tresult PLUGIN_API MyVSTProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
		{
			// input��output�̃o�X��1���ŁA����input��output�̍\�����X�e���I�̏ꍇ
			if (numIns == 1 && numOuts == 1 && inputs[0] == SpeakerArr::kStereo && outputs[0] == SpeakerArr::kStereo)
			{
				return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
			}

			// �Ή����Ă��Ȃ��o�X�\���̏ꍇ�AkResultFalse��Ԃ��B
			return kResultFalse;
		}

		tresult PLUGIN_API MyVSTProcessor::setState(IBStream* state)
		{
			// ���݂�Processor�N���X�̏�Ԃ�Ǎ�
			// �}���`�v���b�g�t�H�[���Ή��ɂ���ꍇ�̓G���f�B�A���ɒ���

			// �ۑ�����Ă���f�[�^��ǂݍ���
			// �ۑ�����Ă���f�[�^����������ꍇ��state->read���J��Ԃ�
			//tresult res;
			//res = state->read(&value, sizeof(ParamValue));
			//if (res != kResultOk)
			//{
			//	// �Ǎ��Ɏ��s�����ꍇ��kResultFalse��Ԃ��B
			//	return kResultFalse;
			//}

			// �֐��̏����ɖ�肪�Ȃ����kResultOk��Ԃ�
			return kResultOk;
		}

		tresult PLUGIN_API MyVSTProcessor::getState(IBStream* state)
		{
			// ���݂�Processor�N���X�̏�Ԃ�ۑ�
			// �}���`�v���b�g�t�H�[���Ή��ɂ���ꍇ�̓G���f�B�A���ɒ���

			// �f�[�^��ۑ�����
			// �ۑ��������f�[�^����������ꍇ��state->write���J��Ԃ��B
			//state->write(&value, sizeof(ParamValue));

			// �֐��̏����ɖ�肪�Ȃ����kResultOk��Ԃ�
			return kResultOk;
		}

		// ===================================================================================
		// �����M������������֐�
		// ===================================================================================
		tresult PLUGIN_API MyVSTProcessor::process(ProcessData& data)
		{
			// �p�����[�^�[�ύX�̏���
			// �^����ꂽ�p�����[�^�[������Ƃ��Adata��inputParameterChanges��
			// IParameterChanges�N���X�ւ̃|�C���^�̃A�h���X������
			if (data.inputParameterChanges != NULL)
			{
				// �^����ꂽ�p�����[�^�[�̐����擾
				int32 paramChangeCount = data.inputParameterChanges->getParameterCount();

				// �^����ꂽ�p�����[�^�[���A�������J��Ԃ��B
				for (int32 i = 0; i < paramChangeCount; i++)
				{
					// �p�����[�^�[�ύX�̃L���[���擾
					// (��������T���v�����ɕ����̃p�����[�^�[�ύX��񂪂���\�������邽�߁A
					// �L���[�Ƃ����`�ɂȂ��Ă���B)
					IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);
					if (queue != NULL)
					{
						// �ǂ̃p�����[�^�[���ύX���ꂽ���m�邽�߁A�p�����[�^�[tag���擾
						int32 tag = queue->getParameterId();

						// �ύX���ꂽ�񐔂��擾
						int32 valueChangeCount = queue->getPointCount();
						ParamValue value;
						int32 sampleOffset;

						// �Ō�ɕύX���ꂽ�l���擾
						if (queue->getPoint(valueChangeCount - 1, sampleOffset, value) == kResultTrue)
						{
							// tag�ɉ��������������{
							switch (tag)
							{
							case PARAM1_TAG:
								// volume�̓����o�[�ϐ��Ƃ��Ă��炩���ߒ�`�E���������Ă����B
								volume = value;
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
			if (eventList != NULL)
			{
				// �C�x���g�̐����擾����B
				int32 numEvent = eventList->getEventCount();
				for (int32 i = 0; i < numEvent; i++)
				{
					// i�Ԗڂ̃C�x���g�f�[�^���擾����
					Event event;
					if (eventList->getEvent(i, event) == kResultOk)
					{
						int16 channel;
						int16 noteNo;
						float velocity;

						// �C�x���g�f�[�^�̃^�C�v���ƂɐU�蕪��
						switch (event.type)
						{
						case Event::kNoteOnEvent: // �m�[�g�I���C�x���g�̏ꍇ
							channel = event.noteOn.channel;
							noteNo = event.noteOn.pitch;
							velocity = event.noteOn.velocity;
							onNoteOn(channel, noteNo, velocity);
							break;

						case Event::kNoteOffEvent: // �m�[�g�I�t�C�x���g�̏ꍇ
							channel = event.noteOff.channel;
							noteNo = event.noteOff.pitch;
							velocity = event.noteOff.velocity;
							onNoteOff(channel, noteNo, velocity);
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
			Sample32* inL = data.inputs[0].channelBuffers32[0];
			Sample32* inR = data.inputs[0].channelBuffers32[1];
			Sample32* outL = data.outputs[0].channelBuffers32[0];
			Sample32* outR = data.outputs[0].channelBuffers32[1];

			// numSamples�Ŏ������T���v�����A��������������
			for (int32 i = 0; i < data.numSamples; i++)
			{
				outL[i] = inL[i];
				outR[i] = inR[i];
			}

			// ���Ȃ����kResultTrue��Ԃ�(�����炭�K��kResultTrue��Ԃ�)
			return kResultTrue;
		}

		void MyVSTProcessor::onNoteOn(int channel, int note, float velocity)
		{
			// MIDI�m�[�g�I���C�x���g�̏������s��

			// �ȒP�ȃT���v���Ȃ̂ŁAchannel��velocity�͖�������

			// �����ꂽ�m�[�g����A�������v�Z
			// �m�[�gNo.69��440Hz�ɂȂ�B�������Ɍv�Z����B
			// �v�Z���̏ڍא����ɂ��Ă͊���
			float pitch = (440.0f * powf(2.0f, (float)(note - (69)) / 12.0));

			// pitchList�̍Ō�ɒǉ�����
			//pitchList.push_back(pitch);

			// �{�����[����0.0f���Ɖ����o�Ȃ��̂ł�1.0f�ɂ��Ă���
			volume = 1.0f;

		}

		void MyVSTProcessor::onNoteOff(int channel, int note, float velocity)
		{
			// MIDI�m�[�g�I�t�C�x���g�̏������s��

			// �ȒP�ȃT���v���Ȃ̂ŁAchannel��velocity�͖�������

			// �����ꂽ�m�[�g����A�������v�Z
			float pitch = (440.0f * powf(2.0f, (float)(note - (69)) / 12.0));

			// pitchList���ŏ����猟�����Apitch�ɍ��v������̂��폜����
			//for (int i = 0; i < (int)pitchList.size(); i++)
			//{
			//	if (pitchList[i] == pitch)
			//	{
			//		// pitch�ƍ��v������̂��������ꍇ�A
			//		// �Y������f�[�^����菜���Č������I������
			//		pitchList.erase(pitchList.begin() + i);
			//		break;
			//	}
			//}

			// pitchList�̃T�C�Y��0�̏ꍇ�A�܂艟���ꂽ�m�[�g���Ȃ��ꍇ�A
			// �{�����[����0.0f�ɂ��ĉ�������
			//if (pitchList.size() == 0)
			//{
			//	volume = 0.0f;
			//}

		}
	}
}
