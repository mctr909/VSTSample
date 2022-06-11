// VST3 SDK�̃C���N���[�h�t�@�C��
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstevents.h" // �C�x���g�o�X���p���ɕK�v

// VST3�쐬�ɕK�v�Ȃ̖��O��Ԃ��g�p
namespace Steinberg {
	namespace Vst {

		// ===================================================================================
		// �����M������������Processor�N���X
		// ===================================================================================
		class MyVSTProcessor : public AudioEffect
		{
		protected:
			ParamValue masterVolume = {0};

		public:
			// �R���X�g���N�^
			MyVSTProcessor();

			// �N���X������������֐�(�K�{)
			tresult PLUGIN_API initialize(FUnknown* context);

			// �N���X���������֐�
			tresult PLUGIN_API terminate();

			// �o�X�\����ݒ肷��֐��B
			tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts);

			// Processor�N���X�̏�Ԃ̓Ǎ��ƕۑ�����֐�
			tresult PLUGIN_API setState(IBStream* state);
			tresult PLUGIN_API getState(IBStream* state);

			// �����M������������֐�(�K�{)
			tresult PLUGIN_API process(ProcessData& data);

			// ����֐�
			// MIDI�m�[�g�I���C�x���g�AMIDI�m�[�g�I�t�C�x���g���󂯎�����ꍇ�ɏ�������֐�
			virtual void onNoteOn(int channel, int note, float velocity);
			virtual void onNoteOff(int channel, int note, float velocity);

			// ����VST Processor�N���X�̃C���X�^���X���쐬���邽�߂̊֐�(�K�{)
			static FUnknown* createInstance(void*) { return (IAudioProcessor*)new MyVSTProcessor(); }
		};

	}
}
