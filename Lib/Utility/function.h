#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <dxgidebug.h>
#include <dxcapi.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

/// <summary>
/// std::string 型の文字列を std::wstring 型に変換する関数。
/// </summary>
/// <param name="str">変換する std::string 型の文字列。</param>
/// <returns>変換された std::wstring 型の文字列。</returns>
std::wstring ConvertString(const std::string& str);

/// <summary>
/// std::wstring 型の文字列を std::string 型に変換する関数。
/// </summary>
/// <param name="str">変換する std::wstring 型の文字列。</param>
/// <returns>変換された std::string 型の文字列。</returns>
std::string ConvertString(const std::wstring& str);

/// <summary>
/// コンソールにメッセージをログ出力する関数。
/// </summary>
/// <param name="message">出力するメッセージ文字列。</param>
void ConsoleLog(const std::string& message);

/// <summary>
/// シェーダーをコンパイルする関数。
/// </summary>
/// <param name="filePath">コンパイルするシェーダーファイルへのパス。</param>
/// <param name="profile">コンパイルに使用するシェーダープロファイル。</param>
/// <param name="dxcUtils">初期化時に生成した IDxcUtils オブジェクト。</param>
/// <param name="dxcCompiler">初期化時に生成した IDxcCompiler3 オブジェクト。</param>
/// <param name="includeHandler">初期化時に生成した IDxcIncludeHandler オブジェクト。</param>
/// <returns>コンパイルされたシェーダーのバイナリデータ（IDxcBlob*）。</returns>
IDxcBlob* CompileShader(
    const std::wstring& filePath,
    const wchar_t* profile,
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler);
