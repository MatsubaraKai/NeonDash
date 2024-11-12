#include "DirectXCommon.h"
#include <thread>
#include "ImGuiCommon.h"
#include <SRVManager.h>
#include <d3dx12.h>

/*----------------------------------------------------------
   このクラスはシングルトンパターンを元に設計する
----------------------------------------------------------*/

/// <summary>
/// ディスクリプタヒープを生成する関数
/// </summary>
/// <param name="heapType">ディスクリプタヒープのタイプ</param>
/// <param name="numDescriptors">ディスクリプタの数</param>
/// <param name="shaderVisible">シェーダーから参照可能かどうか</param>
/// <returns>生成されたディスクリプタヒープのComPtr</returns>
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
    // ディスクリプタヒープの生成
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType; // ヒープのタイプ
    descriptorHeapDesc.NumDescriptors = numDescriptors; // ディスクリプタの数
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    // ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));
    return descriptorHeap;
};

/// <summary>
/// DirectXの初期化を行う関数
/// </summary>
void DirectXCommon::Initialize() {
    InitializeFixFPS();
    // WinAPIの取得
    sWinAPI_ = WinAPI::GetInstance();
    // デバイスの生成
    CreateDevice();

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // やばいエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバックレイヤーとDX12デバックレイヤーの相互作用バグによるエラーメッセージ
            // https://stackoverflow.com/questions/69805245/deirectx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);

        // 解放
        infoQueue->Release();
    }
#endif

    // コマンド関連の初期化
    CreateCommand();

    // スワップチェーンを生成する
    CreateSwapChain();

    // ディスクリプタヒープの生成
    CreateDescriptorHeap();

    // RTVの初期化
    RTVInit();

    // RenderTextureのSRVを設定
    CrateRenderTexture();

    // フェンスの生成
    CreateFence();

    // 深度バッファの初期化
    CreateDepth();

    // ビューポートの初期化
    ViewportInit();

    // シザリング矩形の初期化
    ScissorRectInit();

    // dxCompilerを初期化
    CreateDXCCompilier();
}

/// <summary>
/// 一時的なレンダリングを行う関数
/// </summary>
void DirectXCommon::tempRender()
{
    commandList_->OMSetRenderTargets(1, &rtvHandles_[2], false, nullptr);
    // 指定した色で画面全体をクリアする
    const Vector4 kRenderTargetClearValue{ 0.05f,0.05f,0.05f,1.0f };
    float rederClearColor[] = {
        kRenderTargetClearValue.x,
        kRenderTargetClearValue.y,
        kRenderTargetClearValue.z,
        kRenderTargetClearValue.w
    };

    // TransitionBarrierの設定
    barrier2_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier2_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier2_.Transition.pResource = renderTextureResource_.Get();
    barrier2_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier2_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    // TransitionBarrier
    commandList_->ResourceBarrier(1, &barrier2_);

    // DSVの設定
    dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    commandList_->OMSetRenderTargets(1, &rtvHandles_[2], false, &dsvHandle);

    // 指定した深度で画面全体をクリアする
    commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList_->ClearRenderTargetView(rtvHandles_[2], rederClearColor, 0, nullptr);

    ID3D12DescriptorHeap* heaps[] = { SRVManager::GetInstance()->GetDescriptorHeap().Get() };
    DirectXCommon::GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);

    commandList_->RSSetViewports(1, &tmpViewport);  // ビューポートを設定
    commandList_->RSSetScissorRects(1, &tmpScissorRect); // シザーレクトを設定
}

/// <summary>
/// フレームの開始処理を行う関数
/// </summary>
void DirectXCommon::BeginFrame() {
    // 今回はRenderTargetからPresentにする
    // 遷移前（現在）のResourceState
    barrier2_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    // 遷移後のResourceState
    barrier2_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    // TransitionBarrierを張る
    commandList_->ResourceBarrier(1, &barrier2_);

    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // TransitionBarrierの設定
    barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
    barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    // TransitionBarrier
    commandList_->ResourceBarrier(1, &barrier_);

    // 描画先のRTVを設定する
    commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);

    // 指定した色で画面全体をクリアする
    float clearColor[] = { 0.5f,0.1f,0.25f,1.0f };
    commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);
    dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

    ID3D12DescriptorHeap* heaps[] = { SRVManager::GetInstance()->GetDescriptorHeap().Get() };
    DirectXCommon::GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
    commandList_->RSSetViewports(1, &viewport);  // ビューポートを設定
    commandList_->RSSetScissorRects(1, &scissorRect);    // シザーレクトを設定
}

/// <summary>
/// コマンドリストにImGuiの描画コマンドを積む関数
/// </summary>
void DirectXCommon::ViewChange() {
    HRESULT hr;
    // ImGuiの描画
    ImGuiCommon::GetInstance()->Draw();

    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // TransitionBarrierの設定
    barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    // TransitionBarrierを張る

    commandList_->ResourceBarrier(1, &barrier_);

    // コマンドリストの内容を確定させる
    hr = commandList_->Close();
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, commandLists);

    // GPUとOSに画面の交換を行うよう通知する
    swapChain_->Present(1, 0);

    // フェンスの値を更新
    fenceValue_++;
    // GPUがここまでたどり着いたときに、フェンスの値を指定した値に代入するようにSignalを送る
    commandQueue_->Signal(fence_.Get(), fenceValue_);

    // フェンスの値が指定したSignal値にたどり着いているか確認する
    if (fence_->GetCompletedValue() < fenceValue_)
    {
        // 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        // イベントを待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
    // FPS固定
    UpdateFixFPS();

    // 次のフレーム用のコマンドリストを準備
    hr = commandAllocator_->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr));

};

/// <summary>
/// リソースを解放する関数
/// </summary>
void DirectXCommon::Release() {
    ImGuiCommon::GetInstance()->Release();
    CloseHandle(fenceEvent_);
    CloseWindow(sWinAPI_->GetHwnd());
}

/// <summary>
/// デバイスを生成する関数
/// </summary>
void DirectXCommon::CreateDevice() {
    HRESULT hr;
    // DXGIファクトリーの生成
    dxgiFactory = nullptr;
    hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));

    // 使用するアダプター用の変数。最初にnullptrを入れておく
    useAdapter_ = nullptr;
    // 良い順にアダプターを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
        DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプターの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr));
        // ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 採用したアダプタの情報をログに出力
            ConsoleLog(ConvertString(std::format(L"USE Adapter:{}\n", adapterDesc.Description)));
            break;
        }
        useAdapter_ = nullptr; // ソフトウェアアダプタの場合は見なかったことにする
    }
    // 適切なアダプタが見つからなかったので起動できない
    assert(useAdapter_ != nullptr);

    // D3D12Deviceの生成
    device_ = nullptr;
    // 機能レベルとログの出力の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
    // 高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 採用したアダプターでデバイス生成
        hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
        // 指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(hr)) {
            // 生成できたのでログ出力を行ってループを抜ける
            ConsoleLog(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
            break;
        }
    }
    // デバイスの生成がうまくいかなかったので起動できない
    assert(device_ != nullptr);
    ConsoleLog("Complete create D3D12Device!!!\n"); // 初期化完了のログを出す
}

/// <summary>
/// コマンドキューとコマンドリストを生成する関数
/// </summary>
void DirectXCommon::CreateCommand() {
    HRESULT hr;
    // コマンドキューを生成する
    commandQueue_ = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    hr = device_->CreateCommandQueue(&commandQueueDesc,
        IID_PPV_ARGS(&commandQueue_));
    assert(SUCCEEDED(hr));

    // コマンドアロケータを生成する
    commandAllocator_ = nullptr;
    hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    assert(SUCCEEDED(hr));

    // コマンドリストを生成する
    commandList_ = nullptr;
    hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
        IID_PPV_ARGS(&commandList_));
    assert(SUCCEEDED(hr));
}

/// <summary>
/// スワップチェーンを生成する関数
/// </summary>
void DirectXCommon::CreateSwapChain() {
    HRESULT hr;
    // スワップチェーンの生成
    swapChain_ = nullptr;
    swapChainDesc_.Width = sWinAPI_->GetKClientWidth();
    swapChainDesc_.Height = sWinAPI_->GetKClientHeight();
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc_.SampleDesc.Count = 1; // マルチサンプルしない
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
    swapChainDesc_.BufferCount = 2; // ダブルバッファ
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに映したら中身を破棄
    // コマンドキュー、ウィンドウハンドル設定を渡して生成する
    hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue_.Get(), sWinAPI_->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
    assert(SUCCEEDED(hr));
}

/// <summary>
/// 深度バッファを生成する関数
/// </summary>
void DirectXCommon::CreateDepth() {
    // DepthStencilTextureをウィンドウのサイズで作成
    depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get(), sWinAPI_->GetKClientWidth(), sWinAPI_->GetKClientHeight());

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2D Texture
    // DSVHeapの先頭にDSVを作る
    device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

    // SRVの設定
    depthIndex_ = SRVManager::GetInstance()->Allocate();
    SRVManager::GetInstance()->CreateSRVDepth(depthIndex_, depthStencilResource_.Get(), DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 1);
}

/// <summary>
/// ディスクリプタヒープを生成する関数
/// </summary>
void DirectXCommon::CreateDescriptorHeap() {
    HRESULT hr;

    // ディスクリプタヒープの生成
    // RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないのでShaderVisibleはfalse
    rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvDescriptorSize_, false);
    // DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, dsvDescriptorSize_, false);
    SRVManager* sSRVManager = SRVManager::GetInstance();
    sSRVManager->Init();
    // SwapChainからResourceを引っ張ってくる
    hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
    assert(SUCCEEDED(hr));
}

/// <summary>
/// レンダーターゲットビュー（RTV）を初期化する関数
/// </summary>
void DirectXCommon::RTVInit() {
    // RTVの設定
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして書き込む
    // ディスクリプタヒープの先頭を取得する
    rtvStartHandle_ = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    // 1つ目を作る
    rtvHandles_[0] = rtvStartHandle_;
    device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
    // 2つ目のディスクリプタハンドルを得る
    rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    // 2つ目を作る
    device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);

    // 3つ目のディスクリプタハンドルを得る
    rtvHandles_[2].ptr = rtvHandles_[1].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    const Vector4 kRenderTargetClearValue{ 0.05f,0.05f,0.05f,1.0f }; // 一旦わかりやすいように赤
    renderTextureResource_ = CreateRenderTextureResource(
        device_, WinAPI::kClientWidth_, WinAPI::kClientHeight_,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);
    device_->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc_, rtvHandles_[2]);
}

/// <summary>
/// レンダーテクスチャのSRVを設定する関数
/// </summary>
void DirectXCommon::CrateRenderTexture()
{
    renderTexData_.srvIndex = SRVManager::GetInstance()->Allocate();
    renderTexData_.resource = renderTextureResource_;
    renderTexData_.metaData.mipLevels = 1;
    renderTexData_.metaData.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    SRVManager::GetInstance()->CreateSRVforTexture2D(renderTexData_);
}

/// <summary>
/// フェンスを生成する関数
/// </summary>
void DirectXCommon::CreateFence() {
    HRESULT hr;
    // 初期値0でフェンスを作る
    fence_ = nullptr;
    fenceValue_ = 0;
    hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));

    // フェンスのSignalを待つためのイベントを作成する
    fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent_ != nullptr);
}

/// <summary>
/// ビューポートを初期化する関数
/// </summary>
void DirectXCommon::ViewportInit() {
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = WinAPI::kClientWidth_;
    viewport.Height = WinAPI::kClientHeight_;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    tmpViewport.Width = WinAPI::kClientWidth_;
    tmpViewport.Height = WinAPI::kClientHeight_;
    tmpViewport.TopLeftX = 0;
    tmpViewport.TopLeftY = 0;
    tmpViewport.MinDepth = 0.0f;
    tmpViewport.MaxDepth = 1.0f;
}

/// <summary>
/// シザーレクトを初期化する関数
/// </summary>
void DirectXCommon::ScissorRectInit() {
    // ビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = WinAPI::kClientWidth_;
    scissorRect.top = 0;
    scissorRect.bottom = WinAPI::kClientHeight_;

    tmpScissorRect.left = 0;
    tmpScissorRect.right = WinAPI::kClientWidth_;
    tmpScissorRect.top = 0;
    tmpScissorRect.bottom = WinAPI::kClientHeight_;
}

/// <summary>
/// DXCコンパイラを初期化する関数
/// </summary>
void DirectXCommon::CreateDXCCompilier() {
    HRESULT hr;
    dxcUtils_ = nullptr;
    dxcCompiler_ = nullptr;
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
    assert(SUCCEEDED(hr));

    // includeに対応するための設定を行っておく
    includeHandler_ = nullptr;
    hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    assert(SUCCEEDED(hr));
}

/// <summary>
/// 深度ステンシルの状態をピクセルシェーダーリソースとして読み込み可能に変更する関数
/// </summary>
void DirectXCommon::ChangeDepthStatetoRead()
{
    // TransitionBarrierを設定
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = depthStencilResource_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    // TransitionBarrierを張る
    commandList_->ResourceBarrier(1, &barrier);
}

/// <summary>
/// 深度ステンシルの状態を深度書き込みに変更する関数
/// </summary>
void DirectXCommon::ChangeDepthStatetoRender()
{
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = depthStencilResource_.Get();

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    // TransitionBarrierを張る
    commandList_->ResourceBarrier(1, &barrier);
}

/// <summary>
/// CPUディスクリプタハンドルを取得する関数
/// </summary>
/// <param name="descriptorHeap">ディスクリプタヒープ</param>
/// <param name="descriptorSize">ディスクリプタサイズ</param>
/// <param name="index">ディスクリプタのインデックス</param>
/// <returns>CPUディスクリプタハンドル</returns>
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

/// <summary>
/// GPUディスクリプタハンドルを取得する関数
/// </summary>
/// <param name="descriptorHeap">ディスクリプタヒープ</param>
/// <param name="descriptorSize">ディスクリプタサイズ</param>
/// <param name="index">ディスクリプタのインデックス</param>
/// <returns>GPUディスクリプタハンドル</returns>
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

/// <summary>
/// 深度ステンシル用のテクスチャリソースを生成する関数
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <returns>生成されたリソースのComPtr</returns>
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
    // 生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width; // Textureの幅
    resourceDesc.Height = height; // Textureの高さ
    resourceDesc.MipLevels = 1; // mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f（最大値）でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマットResourceと合わせる

    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
        &depthClearValue, // Clear最適値
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));

    return resource;
}

/// <summary>
/// レンダーテクスチャ用のリソースを生成する関数
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="format">フォーマット</param>
/// <param name="clearColor">クリアカラー</param>
/// <returns>生成されたリソースのComPtr</returns>
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor)
{
    // 生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width; // Textureの幅
    resourceDesc.Height = height; // Textureの高さ
    resourceDesc.MipLevels = 1; // mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
    resourceDesc.Format = format; // フォーマット
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RenderTargetとして使う通知

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

    // カラー値のクリア設定
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format; // フォーマットResourceと合わせる
    clearValue.Color[0] = clearColor.x;
    clearValue.Color[1] = clearColor.y;
    clearValue.Color[2] = clearColor.z;
    clearValue.Color[3] = clearColor.w;

    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // ピクセルシェーダーリソースとして使う
        &clearValue, // Clear最適値
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));

    return resource;
}

/// <summary>
/// FPSを固定するための初期化を行う関数
/// </summary>
void DirectXCommon::InitializeFixFPS()
{
    // 現在時間を記録する
    reference_ = std::chrono::steady_clock::now();
}

/// <summary>
/// FPSを固定するための更新処理を行う関数
/// </summary>
void DirectXCommon::UpdateFixFPS()
{
    // 1/60秒ぴったりの時間
    const std::chrono::microseconds kMinTime(uint64_t(1000000.f / 80.0f));
    // 1/60秒よりわずかに短い時間
    const std::chrono::microseconds kMinCheckTime(uint32_t(1000000.f / 65.0f));
    // 現在時間を取得する
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // 前回記録からの経過時間を取得する
    std::chrono::microseconds elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);
    if (elapsed < kMinTime) {
        // 1/60秒経過するまで微小なスリープを繰り返す
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            // 1マイクロ秒スリープ
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    // 現在の時間を記録する
    reference_ = std::chrono::steady_clock::now();

}

/// <summary>
/// シングルトンインスタンスを取得する関数
/// </summary>
/// <returns>DirectXCommonクラスのインスタンス</returns>
DirectXCommon* DirectXCommon::GetInstance() {
    static DirectXCommon instance;
    return &instance;
}



