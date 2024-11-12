#include "PSOModel.h"

/**
 * @file PSO.cpp
 * @brief パイプラインステートオブジェクト（PSO）を管理するクラスの実装
 */

 /// <summary>
 /// 描画に関する設定をまとめる関数
 /// </summary>
void PSO::CreatePipelineStateObject() {
    // DirectXCommonのインスタンスを取得
    DirectXCommon* sDirectXCommon = DirectXCommon::GetInstance();

    // ルートシグネチャの作成
    PSO::CreateRootSignature();
    // 入力レイアウトの設定
    PSO::SetInputLayout();
    // ブレンドステートの設定
    PSO::SetBlendState();
    // ラスタライザーステートの設定
    PSO::SetRasterizerState();
    // デプスバッファの生成
    PSO::CreateDepth();
    // シェーダーをコンパイルする
    property.vertexShaderBlob = CompileShader(L"Resources/shader/Object3d.VS.hlsl",
        L"vs_6_0", sDirectXCommon->GetDxcUtils(), sDirectXCommon->GetDxcCompiler(), sDirectXCommon->GetIncludeHandler());
    assert(property.vertexShaderBlob != nullptr);

    property.pixelShaderBlob = CompileShader(L"Resources/shader/Object3d.PS.hlsl",
        L"ps_6_0", sDirectXCommon->GetDxcUtils(), sDirectXCommon->GetDxcCompiler(), sDirectXCommon->GetIncludeHandler());
    assert(property.pixelShaderBlob != nullptr);

    graphicsPipelineStateDesc.pRootSignature = property.rootSignature.Get(); // ルートシグネチャ
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // 入力レイアウト
    graphicsPipelineStateDesc.VS = { property.vertexShaderBlob->GetBufferPointer(),
    property.vertexShaderBlob->GetBufferSize() }; // 頂点シェーダー
    graphicsPipelineStateDesc.PS = { property.pixelShaderBlob->GetBufferPointer(),
    property.pixelShaderBlob->GetBufferSize() }; // ピクセルシェーダー
    graphicsPipelineStateDesc.BlendState = blendDesc; // ブレンドステート
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // ラスタライザーステート
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    // 利用するトポロジ（形状）のタイプ。三角形
    graphicsPipelineStateDesc.PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    // どのように画面に色を打ち込むかの設定（気にしなくてよい）
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    // DepthStencilの設定
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // 実際に生成
    property.graphicsPipelineState = nullptr;
    hr_ = sDirectXCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
        IID_PPV_ARGS(&property.graphicsPipelineState));
    assert(SUCCEEDED(hr_));
}

/// <summary>
/// ルートシグネチャを作成する関数
/// </summary>
void PSO::CreateRootSignature() {
    // DirectXCommonのインスタンスを取得
    DirectXCommon* sDirectXCommon = DirectXCommon::GetInstance();

    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // シリアライズしてバイナリにする
    property.signatureBlob = nullptr;

    // ルートパラメータの作成。複数指定できるので配列。今回は6つ
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  // CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  // ピクセルシェーダーで使う
    rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  // CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;  // 頂点シェーダーで使う
    rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

    descriptorRange_[0].BaseShaderRegister = 0; // 0から始まる
    descriptorRange_[0].NumDescriptors = 1; // 数は1つ
    descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
    descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // ディスクリプタテーブルを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダーで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange_[0]; // テーブルの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // テーブルで利用する数

    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[3].Descriptor.ShaderRegister = 1;

    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[4].Descriptor.ShaderRegister = 2;

    descriptorRange_[1].BaseShaderRegister = 1; // 1から始まる
    descriptorRange_[1].NumDescriptors = 1; // 数は1つ
    descriptorRange_[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
    descriptorRange_[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

    rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // ディスクリプタテーブルを使う
    rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダーで使う
    rootParameters[5].DescriptorTable.pDescriptorRanges = &descriptorRange_[1]; // テーブルの中身の配列を指定
    rootParameters[5].DescriptorTable.NumDescriptorRanges = 1; // テーブルで利用する数

    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダーで使う

    descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    property.errorBlob = nullptr;
    hr_ = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &property.signatureBlob, &property.errorBlob);
    if (FAILED(hr_)) {
        ConsoleLog(reinterpret_cast<char*>(property.errorBlob->GetBufferPointer()));
        assert(false);
    }
    // バイナリを元に生成
    property.rootSignature = nullptr;
    hr_ = sDirectXCommon->GetDevice()->CreateRootSignature(0, property.signatureBlob->GetBufferPointer(),
        property.signatureBlob->GetBufferSize(), IID_PPV_ARGS(&property.rootSignature));
    assert(SUCCEEDED(hr_));
}

/// <summary>
/// 入力レイアウトを設定する関数
/// </summary>
void PSO::SetInputLayout() {
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);
}

/// <summary>
/// ブレンドステートを設定する関数
/// </summary>
void PSO::SetBlendState() {
    // ブレンドステートの設定
    // すべての色要素を書き込む
    blendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

/// <summary>
/// ラスタライザーステートを設定する関数
/// </summary>
void PSO::SetRasterizerState() {
    // 裏面（時計回り）を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    // 三角形の中を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}                         

/// <summary>
/// デプスバッファを生成する関数
/// </summary>
void PSO::CreateDepth() {
    // Depthの機能を有効化する
    depthStencilDesc_.DepthEnable = true;
    // 書き込みします
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    // 比較関数はLessEqual。つまり、近づければ描画される
    depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

/// <summary>
/// シングルトンインスタンスを取得する関数
/// </summary>
/// <returns>PSOクラスのインスタンス</returns>
PSO* PSO::GetInstance() {
    static PSO instance;
    return &instance;
}
