#pragma once
#include <vector>
#include <d3d12.h>
#include <PSOProperty.h>

enum PostEffectMode {
	kFullScreen,
	kGrayscale,
	kVignetting,
	kSmoothing,
	kGaussianFilter,
	kOutline,
	kRadialBlur,
	kDissolve,
	kRandom,
	kBloom,
	kNumPostEffect

};
class PostProcess;
// �V�[�����ł̏������s�����N���X
class IPostEffectState {
protected:
	// �V�[���ԍ����Ǘ�����ϐ�
	static int effectNo_;
public:

	/// <summary>
	/// �`��Ɋւ���ݒ���܂Ƃ߂�֐�
	/// </summary>
	virtual PSOProperty CreatePipelineStateObject() = 0;

	virtual void CommandRootParameter(PostProcess* postProcess) = 0;
	virtual void Init() = 0;

	/// <summary>
	/// Shader�̔z��ݒ�
	/// </summary>
	/// <returns></returns>
	virtual std::vector<D3D12_DESCRIPTOR_RANGE> CreateDescriptorRange() = 0;

	/// <summary>
	/// Shader�ɓn���l�����߂�
	/// </summary>
	/// <returns></returns>
	virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParamerter(std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRange) = 0;

	/// <summary>
	/// PS�ɂ̏������ݕ��̐ݒ�
	/// </summary>
	/// <returns></returns>
	virtual std::vector<D3D12_STATIC_SAMPLER_DESC> CreateSampler() = 0;

	/// <summary>
	/// Shader��Resource���֌W�̂��������߂�֐�
	/// </summary>
	virtual D3D12_ROOT_SIGNATURE_DESC CreateRootSignature(PSOProperty& pso, std::vector<D3D12_ROOT_PARAMETER>& rootParameters, std::vector<D3D12_STATIC_SAMPLER_DESC>& samplers) = 0;

	/// <summary>
	/// VS�֓n�����_�f�[�^�̎w����s���֐�
	/// </summary>
	virtual D3D12_INPUT_LAYOUT_DESC SetInputLayout() = 0;

	/// <summary>
	/// PS����̏o�͂��ǂ��������ނ��̐ݒ���s���֐�
	/// </summary>
	virtual D3D12_BLEND_DESC SetBlendState() = 0;

	/// <summary>
	/// ���X�^���C�U�[�Ɋւ���ݒ�̊֐�
	/// </summary>
	virtual D3D12_RASTERIZER_DESC SetRasterrizerState() = 0;

	/// <summary>
	/// DepthBuffer�̐���
	/// </summary>
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepth() = 0;


	// ���z�f�X�g���N�^��p�ӂ��Ȃ��ƌx�������
	virtual ~IPostEffectState();

	// �V�[���ԍ��̃Q�b�^�[
	static int GetEffectNo();
	static void SetEffectNo(int effectNo);


};