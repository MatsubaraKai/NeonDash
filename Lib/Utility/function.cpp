#include "function.h"
#include <Windows.h>
#include <string>
#include <format>
#include <cassert>
//#include "imgui.h"
//#include "imgui_impl_dx12.h"
//#include "imgui_impl_win32.h"

/// <summary>
/// 文字列を std::wstring 型に変換する関数
/// </summary>
/// <param name="str">変換する std::string 型の文字列</param>
/// <returns>変換された std::wstring 型の文字列</returns>
std::wstring ConvertString(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0)
	{
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

/// <summary>
/// 文字列を std::string 型に変換する関数
/// </summary>
/// <param name="str">変換する std::wstring 型の文字列</param>
/// <returns>変換された std::string 型の文字列</returns>
std::string ConvertString(const std::wstring& str)
{
	if (str.empty())
	{
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0)
	{
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

/// <summary>
/// コンソールにログを出力する関数
/// </summary>
/// <param name="message">出力するメッセージ文字列</param>
void ConsoleLog(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

/// <summary>
/// シェーダーをコンパイルする関数
/// </summary>
/// <param name="filePath">コンパイルするシェーダーファイルへのパス</param>
/// <param name="profile">使用するシェーダープロファイル</param>
/// <param name="dxcUtils">初期化時に生成した IDxcUtils オブジェクト</param>
/// <param name="dxcCompiler">初期化時に生成した IDxcCompiler3 オブジェクト</param>
/// <param name="includeHandler">初期化時に生成した IDxcIncludeHandler オブジェクト</param>
/// <returns>コンパイルされたシェーダーのバイナリデータ（IDxcBlob）</returns>
IDxcBlob* CompileShader(
	const std::wstring& filePath,     // コンパイルするシェーダーファイルへのパス
	const wchar_t* profile,           // 使用するシェーダープロファイル
	IDxcUtils* dxcUtils,              // 初期化で生成した IDxcUtils オブジェクト
	IDxcCompiler3* dxcCompiler,       // 初期化で生成した IDxcCompiler3 オブジェクト
	IDxcIncludeHandler* includeHandler) // 初期化で生成した IDxcIncludeHandler オブジェクト
{
	// シェーダーのコンパイル開始をログに出力
	ConsoleLog(ConvertString(std::format(L"Begin CompileShader, path:{}, profile{}\n", filePath, profile)));
	// HLSL ファイルを読み込む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読み込めなかった場合は停止
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8 の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象の HLSL ファイル名
		L"-E", L"main",   // エントリーポイントの指定。基本的に main 以外にはしない
		L"-T", profile,   // シェーダープロファイルの設定
		L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od",  // 最適化を外しておく
		L"-Zpr", // メモリレイアウトは行優先
	};
	// 実際にシェーダーをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer, // 読み込んだファイル
		arguments,           // コンパイルオプション
		_countof(arguments), // コンパイルオプションの数
		includeHandler,      // include が含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなく dxc が起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 警告・エラーが出ていたらログに出して停止
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		ConsoleLog(shaderError->GetStringPointer());
		// 警告・エラーがあれば停止
		assert(false);
	}

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// コンパイル成功のログを出力
	ConsoleLog(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// 使用しないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}
