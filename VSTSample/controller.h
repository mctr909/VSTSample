// VST3 SDK�̃C���N���[�h�t�@�C��
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h" // MIDI��PitchBend��ControlChange���������߂ɕK�v

// ����VST�p�̃C���N���[�h�t�@�C��
#include "myvst3define.h"

// VST3�쐬�ɕK�v�Ȗ��O��Ԃ��g�p
namespace Steinberg {
	namespace Vst {

		// ===================================================================================
		// VST�̃p�����[�^�[�𑀍삷�邽�߂�Controller�N���X
		// ===================================================================================
		class MyVSTController : public EditController, public IMidiMapping // MIDI��PitchBend��ControlChange���������߂ɕK�v
			// MIDI��PitchBend��ControlChange���������߂ɕK�v
		{
		public:
			// �N���X������������֐�(�K�{)
			tresult PLUGIN_API initialize(FUnknown* context);

			// Processor�N���X�̕ۑ���Ԃ𔽉f������֐�
			tresult PLUGIN_API setComponentState(IBStream* state);

			// ����VST Controller�N���X�̃C���X�^���X���쐬���邽�߂̊֐�(�K�{)
			static FUnknown* createInstance(void*) { return (IEditController*)new MyVSTController(); }

			// MIDI��PitchBend��ControlChange�ƃp�����[�^�[���֘A�t����֐�
			virtual tresult PLUGIN_API getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id);

			// VST��{�C���^�[�t�F�C�X(IMidiMapping ��)���I�[�o�[���C�h�����ꍇ�ɕK�v�Ȑ錾(�ڍוs��)
			// DEF_INTERFACE�ɃC���^�[�t�F�C�X���p�����������������Ă����B
			OBJ_METHODS(MyVSTController, EditController)
				DEFINE_INTERFACES
				DEF_INTERFACE(IMidiMapping)
				END_DEFINE_INTERFACES(EditController)
				REFCOUNT_METHODS(EditController)
		};

	}
}
