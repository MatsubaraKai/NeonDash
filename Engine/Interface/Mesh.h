#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxcapi.h>

#include "DirectionLight.h"
#include "TransformationMatrix.h"
#include "wrl.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

/// <summary>
/// @file Mesh.h
/// @brief メッシュクラスの宣言と関連関数。
/// </summary>

/// <summary>
/// メッシュリソースの作成や操作を行うクラス。
/// </summary>
class Mesh
{
public:
    /// <summary>
    /// メッシュクラスのコンストラクタ。
    /// </summary>
    Mesh();

    /// <summary>
    /// メッシュクラスのデストラクタ。
    /// </summary>
    ~Mesh();

    /// <summary>
    /// バッファリソースを作成する静的関数。
    /// </summary>
    /// <param name="device">D3D12デバイス。</param>
    /// <param name="sizeInBytes">バッファのサイズ（バイト単位）。</param>
    /// <returns>作成されたバッファリソースのComPtr。</returns>
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);
};
