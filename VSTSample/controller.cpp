// ����VST�p�̃C���N���[�h�t�@�C��
#include "myvst3define.h"
#include "myvst3fuid.h"
#include "controller.h"

// VST3�쐬�ɕK�v�Ȗ��O��Ԃ��g�p
namespace Steinberg {
	namespace Vst {

		// �N���X������������֐�(�K�{)
		tresult PLUGIN_API MyVSTController::initialize(FUnknown* context)
		{
			// �܂��p�����N���X�̏����������{
			tresult result = EditController::initialize(context);
			if (result == kResultTrue)
			{
				// �p�����[�^�[��ǉ�
				parameters.addParameter(STR16("param1"), STR16("..."), 0, 1, ParameterInfo::kCanAutomate, PARAM_TAG_MASTER_VOLUME);

				// �ȉ��ŗL�̏����������{�B

				// ����͉������Ȃ�
			}

			// �����������������kResultTrue��Ԃ��B
			result = kResultTrue;
			return result;
		}

		tresult PLUGIN_API MyVSTController::setComponentState(IBStream* state)
		{
			// ���݂�Processor�N���X�̏�Ԃ�Ǎ�
			// �}���`�v���b�g�t�H�[���Ή��ɂ���ꍇ�̓G���f�B�A���ɒ���
			tresult res;

			// �ۑ�����Ă���f�[�^��ǂݍ���
			// �ۑ�����Ă���f�[�^����������ꍇ��state->read���J��Ԃ�
			ParamValue value;
			res = state->read(&value, sizeof(ParamValue));
			if (res != kResultOk)
			{
				// �Ǎ��Ɏ��s�����ꍇ��kResultFalse��Ԃ��B
				return kResultFalse;
			}
			// �ǂݍ��܂ꂽ�f�[�^���p�����[�^�[�ɔ��f����
			// ���f����f�[�^��0.0�`1.0�͈̔͂ɂ��Ă���
			value = plainParamToNormalized(PARAM_TAG_MASTER_VOLUME, value);
			setParamNormalized(PARAM_TAG_MASTER_VOLUME, value);

			// �֐��̏����ɖ�肪�Ȃ����kResultOk��Ԃ�
			return kResultOk;
		}

		tresult PLUGIN_API MyVSTController::getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id)
		{

			// midiControllerNumber�Ɋ֌W�t�������p�����[�^�[�^�O(ID)��id�ɑ�����邱�Ƃ�
			// MIDI�R���g���[���`�F���W�ƃp�����[�^�[���֘A�t����
			// �C�x���g�o�X��MIDI�`�����l���ɉ����Ċ֘A�t����ύX�������ꍇ��
			// busIndex��channel���g�p���ď�����������
			switch (midiControllerNumber)
			{
			case kCtrlVolume:
				return kResultTrue;
			case kCtrlExpression:
				return kResultTrue;
			}

			// �֘A�t����p�����[�^�[���Ȃ��ꍇ�AkResultFalse��Ԃ�
			return kResultFalse;
		}
	}
}
