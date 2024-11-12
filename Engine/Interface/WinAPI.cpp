#include "WinAPI.h"
#include <Windows.h>
#include <string>

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx12.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

/*----------------------------------------------------------
   このクラスはシングルトンパターンを元に設計する
----------------------------------------------------------*/

/// <summary>
/// ウィンドウを初期化する関数
/// </summary>
/// <param name="label">ウィンドウのタイトルバーに表示する文字列</param>
void WinAPI::Initialize(const wchar_t* label) {
    // COMライブラリの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);
    // ウィンドウプロシージャを設定
    wc_.lpfnWndProc = WindowProc;
    // ウィンドウクラス名を設定
    wc_.lpszClassName = label;
    // インスタンスハンドルを取得
    wc_.hInstance = GetModuleHandle(nullptr);
    // カーソルを設定
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // ウィンドウクラスを登録する
    RegisterClass(&wc_);

    // ウィンドウサイズを表す構造体にクライアント領域を設定
    RECT wrc = { 0, 0, kClientWidth_, kClientHeight_ };

    // クライアント領域を元にウィンドウ全体のサイズを調整
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウの生成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,        // ウィンドウクラス名
        label,                    // タイトルバーの文字列
        WS_OVERLAPPEDWINDOW,      // ウィンドウスタイル
        CW_USEDEFAULT,            // 表示X座標（デフォルト）
        CW_USEDEFAULT,            // 表示Y座標（デフォルト）
        wrc.right - wrc.left,     // ウィンドウの横幅
        wrc.bottom - wrc.top,     // ウィンドウの縦幅
        nullptr,                  // 親ウィンドウハンドル
        nullptr,                  // メニューハンドル
        wc_.hInstance,            // インスタンスハンドル
        nullptr                   // オプション
    );

#ifdef _DEBUG
    // デバッグコントローラーの取得
    debugController_ = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
        // デバッグレイヤーを有効化する
        debugController_->EnableDebugLayer();
        // GPUベースのバリデーションを有効化する
        debugController_->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    // ウィンドウを表示する
    ShowWindow(hwnd_, SW_SHOW);
};

/// <summary>
/// ウィンドウの解放処理を行う関数
/// </summary>
void WinAPI::Finalize() {
#ifdef _DEBUG
    // デバッグコントローラーを解放
    debugController_->Release();
#endif
    // ウィンドウを閉じる
    CloseWindow(hwnd_);
    // COMライブラリの終了処理
    CoUninitialize();
}

/*-----Accessor-----*/

/// <summary>
/// シングルトンインスタンスを取得する関数
/// </summary>
/// <returns>WinAPIクラスのインスタンス</returns>
WinAPI* WinAPI::GetInstance() {
    static WinAPI instance;
    return &instance;
}

/// <summary>
/// ウィンドウプロシージャ関数
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="msg">メッセージ</param>
/// <param name="wparam">追加の情報（WPARAM）</param>
/// <param name="lparam">追加の情報（LPARAM）</param>
/// <returns>メッセージの処理結果</returns>
LRESULT CALLBACK WinAPI::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // ImGui用のウィンドウプロシージャにメッセージを渡す
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }
    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄されたとき
    case WM_DESTROY:
        // OSに対して、アプリケーション終了を伝える
        PostQuitMessage(0);
        return 0;
    }
    // 既定のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
};

/// <summary>
/// ウィンドウのメッセージを処理する関数
/// </summary>
/// <returns>アプリケーションを終了する場合はtrue、それ以外はfalse</returns>
bool WinAPI::ProcessMessage() {
    MSG msg{};

    // メッセージがあれば取得して処理する
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 終了メッセージが来たらtrueを返す
    if (msg.message == WM_QUIT) {
        return true;
    }
    return false;
}
