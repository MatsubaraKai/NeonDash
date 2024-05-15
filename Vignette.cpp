#include "Vignette.h"

void Vignette::CreatePipelineStateObject() {
	// DirectXCommon�̃C���X�^���X���擾
	DirectXCommon* sDirectXCommon = DirectXCommon::GetInstance();

	Vignette::CreateRootSignature();
	Vignette::SetInputLayout();
	Vignette::SetBlendState();
	Vignette::SetRasterrizerState();
	Vignette::CreateDepth();
	// Shader���R���p�C������
	property.vertexShaderBlob = CompileShader(L"CopyImage.VS.hlsl",
		L"vs_6_0", sDirectXCommon->GetDxcUtils(), sDirectXCommon->GetDxcCompiler(), sDirectXCommon->GetIncludeHandler());
	assert(property.vertexShaderBlob != nullptr);

	property.pixelShaderBlob = CompileShader(L"Vignette.PS.hlsl",
		L"ps_6_0", sDirectXCommon->GetDxcUtils(), sDirectXCommon->GetDxcCompiler(), sDirectXCommon->GetIncludeHandler());
	assert(property.pixelShaderBlob != nullptr);

	graphicsPipelineStateDesc.pRootSignature = property.rootSignature.Get(); // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { property.vertexShaderBlob->GetBufferPointer(),
	property.vertexShaderBlob->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc.PS = { property.pixelShaderBlob->GetBufferPointer(),
	property.pixelShaderBlob->GetBufferSize() };// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //ReterizerState
	// ��������RTV�̏��
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//���p����g�|���W�i�`��j�̃^�C�v�B�O�p�`
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �ǂ̂悤�ɉ�ʂɐF��ł����ނ��̐ݒ�i�C�ɂ��Ȃ��Ă悢�j
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// DeptjStencil�̐ݒ�
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//���ۂɐ���
	property.graphicsPipelineState = nullptr;
	hr_ = sDirectXCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&property.graphicsPipelineState));
	assert(SUCCEEDED(hr_));
}

void Vignette::CreateRootSignature() {
	// DirectXCommon�̃C���X�^���X���擾
	DirectXCommon* sDirectXCommon = DirectXCommon::GetInstance();

	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// �V���A���C�Y���ăo�C�i���ɂ���
	property.signatureBlob = nullptr;

	// RootParmeter�쐬�B�����ł��Ă��ł���̂Ŕz��B����͌��ʂP�����Ȃ̂Œ���1�̔z��
	rootParamerters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  // CBV���g��
	rootParamerters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  // PixelShader�Ŏg��
	rootParamerters[0].Descriptor.ShaderRegister = 0; //���W�X�^�ԍ�0�ƃo�C���h
	rootParamerters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  // CBV���g��
	rootParamerters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;  // vertexShader�Ŏg��
	rootParamerters[1].Descriptor.ShaderRegister = 0; //���W�X�^�ԍ�0�ƃo�C���h

	descriptorRange_[0].BaseShaderRegister = 0; // 0����n�܂�
	descriptorRange_[0].NumDescriptors = 1; // ����1��
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV���g��
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offset�������v�Z

	rootParamerters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescripterTable���g��
	rootParamerters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader�Ŏg��
	rootParamerters[2].DescriptorTable.pDescriptorRanges = descriptorRange_; // Table�̒��g�̔z����w��
	rootParamerters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_); // Table�ŗ��p���鐔


	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // �o�C�i���t�B���^
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1�͈̔͊O�����s�[�g
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // ��r���Ȃ�
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // �����������Mipmap���g��
	staticSamplers[0].ShaderRegister = 0; // ���W�X�^�ԍ�0���g��
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader�Ŏg��

	descriptionRootSignature.pParameters = rootParamerters; // ���[�g�p�����[�^�z��ւ̃|�C���^
	descriptionRootSignature.NumParameters = _countof(rootParamerters); // �z��̒���
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	property.errorBlob = nullptr;
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &property.signatureBlob, &property.errorBlob);
	if (FAILED(hr_)) {
		ConsoleLog(reinterpret_cast<char*>(property.errorBlob->GetBufferPointer()));
		assert(false);
	}
	// �o�C�i�������ɐ���
	property.rootSignature = nullptr;
	hr_ = sDirectXCommon->GetDevice()->CreateRootSignature(0, property.signatureBlob->GetBufferPointer(),
		property.signatureBlob->GetBufferSize(), IID_PPV_ARGS(&property.rootSignature));
	assert(SUCCEEDED(hr_));
}

void Vignette::SetInputLayout() {
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
}

void Vignette::SetBlendState() {
	// blendState�̐ݒ�
	//���ׂĂ̐F�v�f����������
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void Vignette::SetRasterrizerState() {
	//���ʁi���v���j��\�����Ȃ�
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// �O�p�`�̒���h��Ԃ�
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void Vignette::CreateDepth()
{
	// Depth�̋@�\��L��������
	depthStencilDesc_.DepthEnable = true;
	// �������݂��܂�
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// ��r�֐���LessEqual�B�܂�A�߂Â���Ε`�悳���
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}


Vignette* Vignette::GatInstance() {
	static Vignette instance;
	return &instance;
}