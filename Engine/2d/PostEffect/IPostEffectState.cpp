#include "IPostEffectState.h"

// �^�C�g���V�[���ŏ�����
int IPostEffectState::effectNo_ = kBloom;

// ���z�f�X�g���N�^�̒�`
// �������z�֐������Ă��Ȃ��̂ŁA�����Œ�`�ł���
IPostEffectState::~IPostEffectState() {}

// �G�t�F�N�g�ԍ��̃Q�b�^�[
int IPostEffectState::GetEffectNo() { return effectNo_; }

// �G�t�F�N�g�ԍ��̃Z�b�^�[
void IPostEffectState::SetEffectNo(int effectNo) { effectNo_ = effectNo; }
