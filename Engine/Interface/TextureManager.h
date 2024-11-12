#pragma once
#include <d3d12.h>
#include <string>
#include <format>
#include <dxgi1_6.h>
#include <cassert>
#include <dxcapi.h>
#include <wrl.h>
#include "function.h"
#include <unordered_map>
#include <DirectXTex.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

/// <summary>
/// @file TextureManager.h
/// @brief テクスチャの管理を行うクラスの宣言と定義。
/// </summary>

/// <summary>
/// テクスチャ1枚分のデータを表す構造体。
/// </summary>
struct TextureData {
    /// <summary>
    /// テクスチャのメタデータ。
    /// </summary>
    DirectX::TexMetadata metaData;

    /// <summary>
    /// テクスチャのリソース。
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;

    /// <summary>
    /// シェーダーリソースビュー（SRV）のインデックス。
    /// </summary>
    uint32_t srvIndex;

    /// <summary>
    /// テクスチャのCPUディスクリプタハンドル。
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;

    /// <summary>
    /// テクスチャのGPUディスクリプタハンドル。
    /// </summary>
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
};

/*----------------------------------------------------------
   このクラスはシングルトンパターンを元に設計する
----------------------------------------------------------*/

/// <summary>
/// テクスチャの読み込みや管理を行うクラス。
/// </summary>
class TextureManager
{
public:
    /// <summary>
    /// テクスチャマネージャーを初期化する関数。
    /// </summary>
    void Init();

private:
    /// <summary>
    /// コンストラクタ（プライベート）。
    /// </summary>
    TextureManager() = default;

    /// <summary>
    /// デストラクタ。
    /// </summary>
    ~TextureManager() = default;

    /// <summary>
    /// コピー代入演算子を削除（コピー不可）。
    /// </summary>
    const TextureManager& operator=(const TextureManager&) = delete;

public:
    /// <summary>
    /// シングルトンインスタンスを取得する関数。
    /// </summary>
    /// <returns>TextureManagerのインスタンス。</returns>
    static TextureManager* GetInstance();

    /// <summary>
    /// パーティクル用のインデックスを増加させる関数。
    /// </summary>
    /// <returns>更新後のインデックス。</returns>
    static int PlusIndex();

    /// <summary>
    /// テクスチャを読み込み、SRVを作成する関数。
    /// </summary>
    /// <param name="filePath">テクスチャファイルのパス。</param>
    /// <returns>SRVのインデックス。</returns>
    static int StoreTexture(const std::string& filePath);

    /// <summary>
    /// テクスチャマネージャーのリソースを解放する関数。
    /// </summary>
    void Release();

    /// <summary>
    /// テクスチャのメタデータを取得する関数。
    /// </summary>
    /// <param name="filePath">テクスチャファイルのパス。</param>
    /// <returns>テクスチャのメタデータ。</returns>
    const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

    /// <summary>
    /// テクスチャファイルを読み込み、ミップマップを作成する関数。
    /// </summary>
    /// <param name="filePath">テクスチャファイルのパス。</param>
    /// <returns>読み込んだテクスチャのScratchImageオブジェクト。</returns>
    static DirectX::ScratchImage LoadTexture(const std::string& filePath);

    /// <summary>
    /// DirectX12のテクスチャリソースを作成する関数。
    /// </summary>
    /// <param name="device">D3D12デバイス。</param>
    /// <param name="metadata">テクスチャのメタデータ。</param>
    /// <returns>作成されたテクスチャリソース。</returns>
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& matdata);

    /// <summary>
    /// テクスチャデータをGPUにアップロードする関数。
    /// </summary>
    /// <param name="texture">アップロード先のテクスチャリソース。</param>
    /// <param name="mipImages">ミップマップ付きのテクスチャデータ。</param>
    /// <returns>中間リソース（アップロード用バッファ）。</returns>
    static Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource>, const DirectX::ScratchImage& mipImages);

    /// <summary>
    /// パーティクル用のインデックスを取得する関数。
    /// </summary>
    /// <returns>現在のパーティクル用インデックス。</returns>
    uint32_t GetIndex() { return kParIndez; }

private:
    /// <summary>
    /// ディスクリプタの最大数。
    /// </summary>
    static const size_t kNumDescriptors = 256;

    /// <summary>
    /// エラー検知用のHRESULT変数。
    /// </summary>
    HRESULT hr_;

    /// <summary>
    /// 現在空白のディスクリプタヒープ位置を表すインデックス。
    /// </summary>
    static int kSRVIndexTop;

    /// <summary>
    /// パーティクル用のインデックス。
    /// </summary>
    static int kParIndez;

    /// <summary>
    /// テクスチャデータのマップ。キーはファイルパス。
    /// </summary>
    static std::unordered_map<std::string, TextureData> textureDatas_;

    /// <summary>
    /// 中間リソースの配列（テクスチャデータ転送用）。
    /// </summary>
    static Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_[kNumDescriptors];
};
