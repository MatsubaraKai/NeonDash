#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include "function.h"
#include <string>
#include <format>
#include <wrl.h>
#include <chrono>

#include "WinAPI.h"
#include <dxcapi.h>
#include "ResourceObject.h"
#include "Vector4.h"
#include "TextureManager.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

/*----------------------------------------------------------
   このクラスはシングルトンパターンを元に設計する
----------------------------------------------------------*/

/// <summary>
/// @file DirectXCommon.h
/// @brief DirectXの初期化・管理を行うクラスの宣言
/// </summary>
class DirectXCommon final {
public:
    /// <summary>
    /// シングルトンインスタンスを取得する関数
    /// </summary>
    /// <returns>DirectXCommonクラスのインスタンス</returns>
    static DirectXCommon* GetInstance();

    /// <summary>
    /// コンストラクタ
    /// </summary>
    DirectXCommon() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~DirectXCommon() = default;

    /// <summary>
    /// コピー代入演算子を削除（コピー不可）
    /// </summary>
    const DirectXCommon& operator=(const DirectXCommon&) = delete;

    /// <summary>
    /// DirectXの初期化を行う関数
    /// </summary>
    void Initialize();

    /// <summary>
    /// フレームの開始処理を行う関数
    /// </summary>
    void BeginFrame();

    /// <summary>
    /// フレームの終了処理を行う関数
    /// </summary>
    void ViewChange();

    /// <summary>
    /// リソースの解放処理を行う関数
    /// </summary>
    void Release();

    /// <summary>
    /// デバイスを生成する関数
    /// </summary>
    void CreateDevice();

    /// <summary>
    /// コマンド関連の初期化を行う関数
    /// </summary>
    void CreateCommand();

    /// <summary>
    /// スワップチェーンを生成する関数
    /// </summary>
    void CreateSwapChain();

    /// <summary>
    /// 深度バッファを生成する関数
    /// </summary>
    void CreateDepth();

    /// <summary>
    /// ディスクリプタヒープを生成する関数
    /// </summary>
    void CreateDescriptorHeap();

    /// <summary>
    /// レンダーターゲットビュー（RTV）を初期化する関数
    /// </summary>
    void RTVInit();

    /// <summary>
    /// レンダーテクスチャを生成する関数
    /// </summary>
    void CrateRenderTexture();

    /// <summary>
    /// フェンスを生成する関数
    /// </summary>
    void CreateFence();

    /// <summary>
    /// ビューポートを初期化する関数
    /// </summary>
    void ViewportInit();

    /// <summary>
    /// シザーレクトを初期化する関数
    /// </summary>
    void ScissorRectInit();

    /// <summary>
    /// DXCコンパイラを初期化する関数
    /// </summary>
    void CreateDXCCompilier();

    /// <summary>
    /// 深度ステンシルの状態をピクセルシェーダーリソースとして読み込み可能に変更する関数
    /// </summary>
    void ChangeDepthStatetoRead();

    /// <summary>
    /// 深度ステンシルの状態を深度書き込みに変更する関数
    /// </summary>
    void ChangeDepthStatetoRender();

    /// <summary>
    /// 一時的なレンダリングを行う関数
    /// </summary>
    void tempRender();

public: // Getter

    /// <summary>
    /// ディスクリプタハンドルを取得する関数（CPU側）
    /// </summary>
    /// <param name="descriptorHeap">ディスクリプタヒープ</param>
    /// <param name="descriptorSize">ディスクリプタのサイズ</param>
    /// <param name="index">ディスクリプタのインデックス</param>
    /// <returns>CPUディスクリプタハンドル</returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

    /// <summary>
    /// ディスクリプタハンドルを取得する関数（GPU側）
    /// </summary>
    /// <param name="descriptorHeap">ディスクリプタヒープ</param>
    /// <param name="descriptorSize">ディスクリプタのサイズ</param>
    /// <param name="index">ディスクリプタのインデックス</param>
    /// <returns>GPUディスクリプタハンドル</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

    /// <summary>
    /// デバイスを取得する関数
    /// </summary>
    /// <returns>デバイス</returns>
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_.Get(); };

    /// <summary>
    /// コマンドリストを取得する関数
    /// </summary>
    /// <returns>コマンドリスト</returns>
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_.Get(); };

    // アクセサ

    /// <summary>
    /// DXCユーティリティを取得する関数
    /// </summary>
    /// <returns>IDxcUtilsのポインタ</returns>
    IDxcUtils* GetDxcUtils() { return dxcUtils_; };

    /// <summary>
    /// DXCコンパイラを取得する関数
    /// </summary>
    /// <returns>IDxcCompiler3のポインタ</returns>
    IDxcCompiler3* GetDxcCompiler() { return dxcCompiler_; };

    /// <summary>
    /// インクルードハンドラを取得する関数
    /// </summary>
    /// <returns>IDxcIncludeHandlerのポインタ</returns>
    IDxcIncludeHandler* GetIncludeHandler() { return includeHandler_; };

    /// <summary>
    /// スワップチェーンの設定を取得する関数
    /// </summary>
    /// <returns>DXGI_SWAP_CHAIN_DESC1構造体</returns>
    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; };

    /// <summary>
    /// レンダーターゲットビューの設定を取得する関数
    /// </summary>
    /// <returns>D3D12_RENDER_TARGET_VIEW_DESC構造体</returns>
    D3D12_RENDER_TARGET_VIEW_DESC GetrtvDesc() { return rtvDesc_; };

    /// <summary>
    /// 深度ステンシルの設定を取得する関数
    /// </summary>
    /// <returns>D3D12_DEPTH_STENCIL_DESC構造体</returns>
    D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc() { return depthStencilDesc_; };

    /// <summary>
    /// レンダーテクスチャのSRVインデックスを取得する関数
    /// </summary>
    /// <returns>SRVのインデックス</returns>
    uint32_t GetRenderIndex() { return renderTexData_.srvIndex; }

    /// <summary>
    /// 深度バッファのSRVインデックスを取得する関数
    /// </summary>
    /// <returns>SRVのインデックス</returns>
    uint32_t GetDepthIndex() { return depthIndex_; }

public:
    // リソースリークチェック用の構造体
    struct D3DResourceLeakChecker {
        /// <summary>
        /// デストラクタでリソースリークチェックを行う
        /// </summary>
        ~D3DResourceLeakChecker()
        {
            // リソースリークチェック
            Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
                debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
            }
        }
    };

    /// <summary>
    /// ディスクリプタヒープを生成する関数
    /// </summary>
    /// <param name="heapType">ヒープのタイプ</param>
    /// <param name="numDescriptors">ディスクリプタの数</param>
    /// <param name="shaderVisible">シェーダーから参照可能かどうか</param>
    /// <returns>生成されたディスクリプタヒープ</returns>
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

    /// <summary>
    /// 深度ステンシル用のテクスチャリソースを生成する関数
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    /// <returns>生成されたリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

    /// <summary>
    /// レンダーテクスチャ用のリソースを生成する関数
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="width">幅</param>
    /// <param name="height">高さ</param>
    /// <param name="format">フォーマット</param>
    /// <param name="clearColor">クリアカラー</param>
    /// <returns>生成されたリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor);

    /*
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
        D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        handleCPU.ptr += (descriptorSize * index);
        return handleCPU;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
        D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
        handleGPU.ptr += (descriptorSize * index);
        return handleGPU;
    }
    */

private:
    /// <summary>
    /// FPS固定のための初期化を行う関数
    /// </summary>
    void InitializeFixFPS();

    /// <summary>
    /// FPS固定のための更新処理を行う関数
    /// </summary>
    void UpdateFixFPS();

    /// <summary>
    /// FPS固定用の時間を記録する変数
    /// </summary>
    std::chrono::steady_clock::time_point reference_;

    /// <summary>
    /// DXGIファクトリー
    /// </summary>
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

    // HRESULTはWindows系のエラーコードであり、
    // 関数が成功したかどうかをSUCCEEDEDマクロで判定できる

    /// <summary>
    /// 使用するアダプター
    /// </summary>
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

    /// <summary>
    /// デバイス
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Device> device_;

    /// <summary>
    /// コマンドキュー
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;

    /// <summary>
    /// コマンドアロケータ
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;

    /// <summary>
    /// コマンドリスト
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    /// <summary>
    /// スワップチェーン
    /// </summary>
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;

    /// <summary>
    /// スワップチェーンの設定
    /// </summary>
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

    /// <summary>
    /// スワップチェーンのリソース（バックバッファ）
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };

    /// <summary>
    /// レンダーテクスチャリソース
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_ = nullptr;

    /// <summary>
    /// レンダーターゲットビュー（RTV）のディスクリプタヒープ
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

    /// <summary>
    /// 深度ステンシルビュー（DSV）のディスクリプタヒープ
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

    /// <summary>
    /// レンダーターゲットビューの設定
    /// </summary>
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

    /// <summary>
    /// RTVディスクリプタヒープの先頭ハンドル
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle_;

    /// <summary>
    /// RTVハンドル（複数のバックバッファ分）
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[3];

    /// <summary>
    /// リソースバリア（通常のバリア）
    /// </summary>
    D3D12_RESOURCE_BARRIER barrier_{};

    /// <summary>
    /// リソースバリア（追加のバリア）
    /// </summary>
    D3D12_RESOURCE_BARRIER barrier2_{};

    /// <summary>
    /// フェンス
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

    /// <summary>
    /// フェンスの値
    /// </summary>
    uint64_t fenceValue_;

    /// <summary>
    /// フェンスイベントハンドル
    /// </summary>
    HANDLE fenceEvent_;

    /// <summary>
    /// DXCユーティリティ
    /// </summary>
    IDxcUtils* dxcUtils_;

    /// <summary>
    /// DXCコンパイラ
    /// </summary>
    IDxcCompiler3* dxcCompiler_;

    /// <summary>
    /// インクルードハンドラ
    /// </summary>
    IDxcIncludeHandler* includeHandler_;

    /// <summary>
    /// WinAPIインスタンス
    /// </summary>
    WinAPI* sWinAPI_ = nullptr;

    /// <summary>
    /// 深度ステンシルリソース
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

    /// <summary>
    /// 深度ステンシルの設定
    /// </summary>
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

    /// <summary>
    /// DSVハンドル
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

    /// <summary>
    /// ビューポート
    /// </summary>
    D3D12_VIEWPORT viewport{};

    /// <summary>
    /// シザーレクト
    /// </summary>
    D3D12_RECT scissorRect{};

    /// <summary>
    /// 一時的なビューポート
    /// </summary>
    D3D12_VIEWPORT tmpViewport{};

    /// <summary>
    /// 一時的なシザーレクト
    /// </summary>
    D3D12_RECT tmpScissorRect{};

    /// <summary>
    /// レンダーテクスチャデータ
    /// </summary>
    TextureData renderTexData_;

    /// <summary>
    /// 深度バッファのSRVインデックス
    /// </summary>
    uint32_t depthIndex_;

public: // 共通変数　以下の変数は変更しない
    /// <summary>
    /// RTVディスクリプタヒープのサイズ
    /// </summary>
    const static uint32_t rtvDescriptorSize_ = 3;

    /// <summary>
    /// DSVディスクリプタヒープのサイズ
    /// </summary>
    const static uint32_t dsvDescriptorSize_ = 1;
};

