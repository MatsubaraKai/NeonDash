#pragma once
#include <d3d12.h>
#include "DirectXCommon.h"
#include "PSOProperty.h"
#include "Vector3.h"


struct CameraForGPU {
	Vector3 worldPosition;
};

class PSO : public PSOProperty
{
public:
	static PSO* GatInstance();

	PSO() = default;
	~PSO() = default;
	const PSO& operator=(const PSO&) = delete;


	/// <summary>
	/// �`��Ɋւ���ݒ���܂Ƃ߂�֐�
	/// </summary>
	void CreatePipelineStateObject();

	/// <summary>
	/// Shader��Resource���֌W�̂��������߂�֐�
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// VS�֓n�����_�f�[�^�̎w����s���֐�
	/// </summary>
	void SetInputLayout();

	/// <summary>
	/// PS����̏o�͂��ǂ��������ނ��̐ݒ���s���֐�
	/// </summary>
	void SetBlendState();

	/// <summary>
	/// ���X�^���C�U�[�Ɋւ���ݒ�̊֐�
	/// </summary>
	void SetRasterrizerState();

	/// <summary>
	/// Release
	/// </summary>
	/// <returns></returns>

	/// <summary>
	/// DepthBuffer�̐���
	/// </summary>
	void CreateDepth();


	PSOProperty GetProperty() { return property; }

private:
	HRESULT hr_;
	// RootSignature�쐬
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	// �V���A���C�Y���ăo�C�i���ɂ���
	//ID3DBlob* signatureBlob;
	//ID3DBlob* errorBlob;
	// �o�C�i�������ɐ���
	//ID3D12RootSignature* rootSignature;
	// RootParmeter�쐬�B�����ł��Ă��ł���̂Ŕz��B����͌��ʂP�����Ȃ̂Œ���1�̔z��
	D3D12_ROOT_PARAMETER rootParamerters[6] = {};

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};
	D3D12_INPUT_LAYOUT_DESC  inputLayoutDesc{};

	// blendState�̐ݒ�
	D3D12_BLEND_DESC blendDesc{};

	// RasiterzerState�̐ݒ�
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	/*IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//���ۂɐ���
	//ID3D12PipelineState* graphicsPipelineState;

	// DepthStencilState�̐ݒ�
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

	// �`����RTV��ݒ肷��
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	PSOProperty property;

};
