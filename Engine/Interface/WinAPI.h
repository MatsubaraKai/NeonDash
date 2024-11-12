#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

/*----------------------------------------------------------
	   このクラスはシングルトンパターンを元に設計する
----------------------------------------------------------*/

/**
 * @file WinAPI.h
 * @brief Windowsアプリケーションの初期化・管理を行うクラスの宣言。
 */

 /// <summary>
 /// Windows APIを使用したウィンドウの作成と管理を行うクラス。
 /// </summary>
class WinAPI final
{
public:
	/// <summary>
	/// ウィンドウプロシージャ関数（静的メンバ関数）。
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル。</param>
	/// <param name="msg">メッセージ。</param>
	/// <param name="wparam">追加の情報（WPARAM）。</param>
	/// <param name="lparam">追加の情報（LPARAM）。</param>
	/// <returns>メッセージの処理結果。</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	/// <summary>
	/// コンストラクタ（プライベート）。
	/// </summary>
	WinAPI() = default;

	/// <summary>
	/// デストラクタ。
	/// </summary>
	~WinAPI() = default;

	/*-----Default Method-----*/

	/// <summary>
	/// ウィンドウの初期化を行う関数。
	/// </summary>
	/// <param name="label">ウィンドウのタイトルバーに表示する文字列。</param>
	void Initialize(const wchar_t* label);

	/// <summary>
	/// ウィンドウの解放処理を行う関数。
	/// </summary>
	void Finalize();

	/*-----User Method-----*/

	/// <summary>
	/// シングルトンパターンの実装。
	/// コピーコンストラクタを削除（コピー不可）。
	/// </summary>
	WinAPI(const WinAPI& obj) = delete;

	/// <summary>
	/// 代入演算子を削除（コピー不可）。
	/// </summary>
	WinAPI& operator=(const WinAPI& obj) = delete;

	/// <summary>
	/// シングルトンインスタンスを取得する関数。
	/// </summary>
	/// <returns>WinAPIクラスのインスタンス。</returns>
	static WinAPI* GetInstance();

	/// <summary>
	/// クライアント領域の横幅を取得する関数。
	/// </summary>
	/// <returns>クライアント領域の横幅。</returns>
	static int32_t GetKClientWidth() { return kClientWidth_; };

	/// <summary>
	/// クライアント領域の縦幅を取得する関数。
	/// </summary>
	/// <returns>クライアント領域の縦幅。</returns>
	static int32_t GetKClientHeight() { return kClientHeight_; };

	/// <summary>
	/// ウィンドウハンドルを取得する関数。
	/// </summary>
	/// <returns>ウィンドウハンドル（HWND）。</returns>
	HWND GetHwnd() { return hwnd_; }

	/// <summary>
	/// ウィンドウクラスを取得する関数。
	/// </summary>
	/// <returns>ウィンドウクラス（WNDCLASS）。</returns>
	WNDCLASS GetWc() { return wc_; }

	/// <summary>
	/// メッセージの処理を行う関数。
	/// </summary>
	/// <returns>アプリケーションを終了する場合はtrue、それ以外はfalse。</returns>
	bool ProcessMessage();

public: // 定数
	/// <summary>
	/// クライアント領域の横幅。
	/// </summary>
	const static int32_t kClientWidth_ = 1280;

	/// <summary>
	/// クライアント領域の縦幅。
	/// </summary>
	const static int32_t kClientHeight_ = 720;

private:
	/// <summary>
	/// ウィンドウのハンドル。
	/// </summary>
	HWND hwnd_;

	/// <summary>
	/// ウィンドウクラス構造体。
	/// </summary>
	WNDCLASS wc_{};

#ifdef _DEBUG
	/// <summary>
	/// デバッグコントローラー（デバッグ時のみ）。
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_;
#endif
};
