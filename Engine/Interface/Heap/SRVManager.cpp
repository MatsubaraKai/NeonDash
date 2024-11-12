#include "SRVManager.h"
#include "DirectXCommon.h"

/**
 * @file SRVManager.cpp
 * @brief シェーダーリソースビュー（SRV）を管理するクラスの実装
 */

 /// <summary>
 /// SRVの最大数を定義する定数
 /// </summary>
const uint32_t SRVManager::kMaXSRVCount = 512;

/// <summary>
/// 現在使用中のインデックス
/// </summary>
uint32_t SRVManager::useIndex_ = 1;

/// <summary>
/// SRV用のデスクリプタサイズ
/// </summary>
uint32_t SRVManager::descriptorSize_;

/// <summary>
/// SRV用のデスクリプタヒープ
/// </summary>
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVManager::descriptorHeap_;

/// <summary>
/// SRVManagerの初期化を行う関数
/// </summary>
void SRVManager::Init() {
    // デスクリプタヒープの生成
    descriptorHeap_ = DirectXCommon::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaXSRVCount, true);
    // デスクリプタ1個分のサイズを取得して記録
    descriptorSize_ = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

/// <summary>
/// SRVのインデックスを割り当てる関数
/// </summary>
/// <returns>割り当てられたインデックス</returns>
uint32_t SRVManager::Allocate()
{
    // 割り当てるインデックスを一時的に保存
    int index = useIndex_;
    // 次回のためにインデックスを1進める
    useIndex_++;
    // 割り当てたインデックスを返す
    return index;
}

/// <summary>
/// SRVManagerのシングルトンインスタンスを取得する関数
/// </summary>
/// <returns>SRVManagerのインスタンス</returns>
SRVManager* SRVManager::GetInstance()
{
    static SRVManager instance;
    return &instance;
}

/// <summary>
/// 指定したインデックスのCPUデスクリプタハンドルを取得する関数
/// </summary>
/// <param name="index">取得するインデックス</param>
/// <returns>CPUデスクリプタハンドル</returns>
D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUDescriptorHandle(uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize_ * index);
    return handleCPU;
}

/// <summary>
/// 指定したインデックスのGPUデスクリプタハンドルを取得する関数
/// </summary>
/// <param name="index">取得するインデックス</param>
/// <returns>GPUデスクリプタハンドル</returns>
D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUDescriptorHandle(uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize_ * index);
    return handleGPU;
}

/// <summary>
/// 2Dテクスチャ用のSRVを作成する関数
/// </summary>
/// <param name="textureData">テクスチャデータ</param>
void SRVManager::CreateSRVforTexture2D(const TextureData& textureData)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    // metaDataを基にSRVの設定
    srvDesc.Format = textureData.metaData.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    if (textureData.metaData.IsCubemap()) {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MostDetailedMip = 0; // unionがTextureCubeになったが、内部パラメータの意味はTexture2Dと変わらない
        srvDesc.TextureCube.MipLevels = UINT_MAX;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    }
    else {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = UINT(textureData.metaData.mipLevels);
    }
    // SRVの生成
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, GetCPUDescriptorHandle(textureData.srvIndex));
}

/// <summary>
/// 構造化バッファ用のSRVを作成する関数
/// </summary>
/// <param name="srvIndex">SRVのインデックス</param>
/// <param name="pResource">リソースへのポインタ</param>
/// <param name="numElements">要素数</param>
/// <param name="structureByteStride">ストラクチャのバイトサイズ</param>
void SRVManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = sizeof(structureByteStride);
    // SRVの生成
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

/// <summary>
/// デプスバッファ用のSRVを作成する関数
/// </summary>
/// <param name="srvIndex">SRVのインデックス</param>
/// <param name="pResource">リソースへのポインタ</param>
/// <param name="Format">フォーマット</param>
/// <param name="MipLevels">ミップレベル数</param>
void SRVManager::CreateSRVDepth(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    // metaDataを基にSRVの設定
    srvDesc.Format = Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = UINT(MipLevels);
    // SRVの生成
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

/// <summary>
/// レンダーテクスチャ用のSRVを作成する関数
/// </summary>
/// <param name="srvIndex">SRVのインデックス</param>
/// <param name="pResource">リソースへのポインタ</param>
/// <param name="Format">フォーマット</param>
/// <param name="MipLevels">ミップレベル数</param>
void SRVManager::CreateSRVRenderTexture(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    // metaDataを基にSRVの設定
    srvDesc.Format = Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = UINT(MipLevels);
    // SRVの生成
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

/// <summary>
/// 描画の前準備として、デスクリプタヒープを設定する関数
/// </summary>
void SRVManager::PreDraw()
{
    // 描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
    DirectXCommon::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

/// <summary>
/// グラフィックスルートディスクリプタテーブルを設定する関数
/// </summary>
/// <param name="rootParameterIndex">ルートパラメータのインデックス</param>
/// <param name="srvIndex">SRVのインデックス</param>
void SRVManager::SetGraphicsRootDescriptortable(UINT rootParameterIndex, uint32_t srvIndex)
{
    DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(rootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}
