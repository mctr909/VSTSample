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
				parameters.addParameter(STR16("Master Volume"), STR16("..."), 0, 1, ParameterInfo::kCanAutomate, PARAM_TAG_MASTER_VOLUME);

				for (int c = 0; c < CHANNEL_COUNT; c++) {
					parameters.addParameter(STR16("Ch Vol."), STR16("..."), 128, 1, ParameterInfo::kCanAutomate, PARAM_TAG_CHANNEL_VOL + c);
					parameters.addParameter(STR16("Ch Exp."), STR16("..."), 128, 1, ParameterInfo::kCanAutomate, PARAM_TAG_CHANNEL_EXP + c);
					parameters.addParameter(STR16("Ch Pan"), STR16("..."), 128, 1, ParameterInfo::kCanAutomate, PARAM_TAG_CHANNEL_PAN + c);
				}

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
				id = PARAM_TAG_CHANNEL_VOL + channel;
				return kResultTrue;
			case kCtrlExpression:
				id = PARAM_TAG_CHANNEL_EXP + channel;
				return kResultTrue;
			case kCtrlPan:
				id = PARAM_TAG_CHANNEL_PAN + channel;
				return kResultTrue;
			}

			// �֘A�t����p�����[�^�[���Ȃ��ꍇ�AkResultFalse��Ԃ�
			return kResultFalse;
		}
	}
}
