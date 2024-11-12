#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include "TextureManager.h"

/**
 * @file SRVManager.h
 * @brief シェーダーリソースビュー（SRV）を管理するクラス
 */

 /*----------------------------------------------------------
    このクラスはシングルトンパターンを元に設計する
 --------------------------------------------------------------*/

 /// <summary>
 /// シェーダーリソースビュー（SRV）を管理するクラス
 /// </summary>
class SRVManager
{
public:
    /// <summary>
    /// SRVManagerの初期化を行う関数
    /// </summary>
    void Init();

    /// <summary>
    /// SRVのインデックスを割り当てる関数
    /// </summary>
    /// <returns>割り当てられたインデックス</returns>
    static uint32_t Allocate();

private:
    /// <summary>
    /// コンストラクタ（シングルトンパターンのためプライベート）
    /// </summary>
    SRVManager() = default;

    /// <summary>
    /// デストラクタ（シングルトンパターンのためプライベート）
    /// </summary>
    ~SRVManager() = default;

    /// <summary>
    /// コピー代入演算子を削除（コピー不可）
    /// </summary>
    /// <param name="other">コピー元のオブジェクト</param>
    /// <returns>コピー不可</returns>
    const SRVManager& operator=(const SRVManager&) = delete;

public:
    /// <summary>
    /// シングルトンのインスタンスを取得する関数
    /// </summary>
    /// <returns>SRVManagerのインスタンス</returns>
    static SRVManager* GetInstance();

    /// <summary>
    /// デスクリプタヒープを取得する関数
    /// </summary>
    /// <returns>デスクリプタヒープのComPtr</returns>
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_.Get(); };

    /// <summary>
    /// 最大SRV数（最大テクスチャ枚数）
    /// </summary>
    static const uint32_t kMaXSRVCount;

    /// <summary>
    /// SRV用のデスクリプタサイズ
    /// </summary>
    static uint32_t descriptorSize_;

    /// <summary>
    /// SRV用のデスクリプタヒープ
    /// </summary>
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

    /// <summary>
    /// 次に使用するSRVインデックス
    /// </summary>
    static uint32_t useIndex_;

    /// <summary>
    /// 指定したインデックスのCPUデスクリプタハンドルを取得する関数
    /// </summary>
    /// <param name="index">取得するインデックス</param>
    /// <returns>CPUデスクリプタハンドル</returns>
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

    /// <summary>
    /// 指定したインデックスのGPUデスクリプタハンドルを取得する関数
    /// </summary>
    /// <param name="index">取得するインデックス</param>
    /// <returns>GPUデスクリプタハンドル</returns>
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

    /// <summary>
    /// 2Dテクスチャ用のSRVを作成する関数
    /// </summary>
    /// <param name="textureData">テクスチャデータ</param>
    static void CreateSRVforTexture2D(const TextureData& textureData);

    /// <summary>
    /// 構造化バッファ用のSRVを作成する関数
    /// </summary>
    /// <param name="srvIndex">SRVのインデックス</param>
    /// <param name="pResource">リソースへのポインタ</param>
    /// <param name="numElements">要素数</param>
    /// <param name="structureByteStride">構造体のバイトサイズ</param>
    static void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

    /// <summary>
    /// レンダーテクスチャ用のSRVを作成する関数
    /// </summary>
    /// <param name="srvIndex">SRVのインデックス</param>
    /// <param name="pResource">リソースへのポインタ</param>
    /// <param name="Format">フォーマット</param>
    /// <param name="MipLevels">ミップレベル数</param>
    static void CreateSRVRenderTexture(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

    /// <summary>
    /// デプスバッファ用のSRVを作成する関数
    /// </summary>
    /// <param name="srvIndex">SRVのインデックス</param>
    /// <param name="pResource">リソースへのポインタ</param>
    /// <param name="Format">フォーマット</param>
    /// <param name="MipLevels">ミップレベル数</param>
    static void CreateSRVDepth(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

    //static void CreateSRVRenderTexture(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

    /// <summary>
    /// 描画の前準備として、デスクリプタヒープを設定する関数
    /// </summary>
    void PreDraw();

    /// <summary>
    /// グラフィックスルートディスクリプタテーブルを設定する関数
    /// </summary>
    /// <param name="rootParameterIndex">ルートパラメータのインデックス</param>
    /// <param name="srvIndex">SRVのインデックス</param>
    void SetGraphicsRootDescriptortable(UINT rootParameterIndex, uint32_t srvIndex);
};
