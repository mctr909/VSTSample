// VST3 SDK�̃C���N���[�h�t�@�C��
#include "public.sdk/source/main/pluginfactory.h"

// ����VST�̃w�b�_�t�@�C�����C���N���[�h
#include "myvst3fuid.h"
#include "processor.h"
#include "controller.h"

// �����(������)�̖��O�B�I�[�����u\0�v�܂߂�64�����܂ŁB
#define MYVST_VENDOR   "C++ de VST"

// �����(������)��Web�T�C�g��URL�B�I�[�����u\0�v�܂߂�256�����܂ŁB
#define MYVST_URL      "https://www.utsbox.com/"

// �����(������)�̘A���惁�[���A�h���X�B�I�[�����u\0�v�܂߂�128�����܂ŁB
#define MYVST_EMAIL    "mailto:xxxx@example.com"

// ���삷��VST�̖��O�B�I�[�����u\0�v�܂߂�64�����܂ŁB
#define MYVST_VSTNAME  "MyVST Name"

// ���삷��VST�̃o�[�W�����B�I�[�����u\0�v�܂߂�64�����܂ŁB
#define MYVST_VERSION  "0" 

// ���삷��VST�̃J�e�S���B�I�[�����u\0�v�܂߂�64�����܂ŁB
#define MYVST_SUBCATEGORIES Vst::PlugType::kInstrument


// ===================================================================================
// DLL�t�@�C���̏������A�I�������֐�
// ===================================================================================
// ��{�I�ɉ�������K�v�͂Ȃ��B(VST SDK 3.7.1���O�̃o�[�W�����̏ꍇ�̓R�����g���O������)
//bool InitModule() { return true; }
//bool DeinitModule() { return true; }


// ===================================================================================
// ����VST�v���O�C���̐���
// ===================================================================================
BEGIN_FACTORY_DEF(MYVST_VENDOR, MYVST_URL, MYVST_EMAIL)

// MyVSTProcessor�N���X�̍쐬���s��
DEF_CLASS2(
	INLINE_UID_FROM_FUID(Steinberg::Vst::ProcessorUID),
	PClassInfo::kManyInstances,
	kVstAudioEffectClass,
	MYVST_VSTNAME,
	Vst::kDistributable,
	MYVST_SUBCATEGORIES,
	MYVST_VERSION,
	kVstVersionString,
	Steinberg::Vst::MyVSTProcessor::createInstance
)

// MyVSTController�N���X�̍쐬���s��
DEF_CLASS2(
	INLINE_UID_FROM_FUID(Steinberg::Vst::ControllerUID),
	PClassInfo::kManyInstances,
	kVstComponentControllerClass,
	MYVST_VSTNAME " Controller",	// ����VST�̖��O��"Controller"��t�^�������́B
	0,						        // �g��Ȃ��̂ŕK��0�ɂ���
	"",						        // �g��Ȃ��̂ŕK��""�ɂ���
	MYVST_VERSION,
	kVstVersionString,
	Steinberg::Vst::MyVSTController::createInstance
)

END_FACTORY