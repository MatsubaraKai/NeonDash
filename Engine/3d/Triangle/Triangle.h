#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxcapi.h>

#include "WinAPI.h"
#include "DirectXCommon.h"
#include "Mesh.h"
#include "PSOModel.h"
#include "VertexData.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "DirectionLight.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "WorldTransform.h"
#include "Camera.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

/**
* @file Triangle.h
* @brief 三角形を描画するためのクラス
*/

class Triangle
{
public:
    /// <summary>
    /// 初期化処理を行う
    /// </summary>
    /// <param name="camera">カメラオブジェクトへのポインタ</param>
    /// <param name="DrawColor">描画色</param>
    void Initialize(Camera* camera, Vector4 DrawColor);

    /// <summary>
    /// 更新処理を行う
    /// </summary>
    /// <param name="camera">カメラオブジェクト</param>
    /// <param name="DrawColor">描画色</param>
    void Update(Camera camera, Vector4 DrawColor);

    /// <summary>
    /// 描画処理を行う
    /// </summary>
    /// <param name="worldTransform">ワールド変換</param>
    /// <param name="camera">カメラオブジェクトへのポインタ</param>
    /// <param name="texture">テクスチャハンドル</param>
    /// <param name="DrawColor">描画色</param>
    void Draw(WorldTransform worldTransform, Camera* camera, uint32_t texture, Vector4 DrawColor);

    /// <summary>
    /// リソースの解放を行う
    /// </summary>
    void Release();

    /// <summary>
    /// 頂点バッファビューを作成する
    /// </summary>
    /// <returns>頂点バッファビュー</returns>
    D3D12_VERTEX_BUFFER_VIEW CreateBufferView();

    /// <summary>
    /// バッファリソースのディスクリプタを作成する
    /// </summary>
    /// <param name="sizeInBytes">バッファサイズ（バイト単位）</param>
    /// <returns>バッファリソースディスクリプタ</returns>
    D3D12_RESOURCE_DESC CreateBufferResourceDesc(size_t sizeInBytes);

    /// <summary>
    /// 処理結果を格納するHRESULT
    /// </summary>
    HRESULT hr;

    /* 頂点用 */
    /// <summary>
    /// 頂点リソース（GPUメモリ上の頂点バッファ）
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

    /// <summary>
    /// 頂点バッファビュー（描画時に使用するビュー）
    /// </summary>
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

    /// <summary>
    /// 頂点データへのポインタ
    /// </summary>
    VertexData* vertexData_;

    /* 色用 */
    /// <summary>
    /// マテリアルリソース（GPUメモリ上のマテリアルデータ）
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

    /// <summary>
    /// マテリアルバッファビュー
    /// </summary>
    D3D12_VERTEX_BUFFER_VIEW materialBufferView{};

    /// <summary>
    /// マテリアルデータへのポインタ
    /// </summary>
    Material* materialData;

    /* 移動用 */
    /// <summary>
    /// WVP（ワールド・ビュー・プロジェクション）用のリソース
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

    /// <summary>
    /// WVPデータへのポインタ
    /// </summary>
    TransformationMatrix* TransformationData;

    /// <summary>
    /// WVPの頂点バッファビュー
    /// </summary>
    D3D12_VERTEX_BUFFER_VIEW wvpBufferView{};

    /* 平行光源用 */
    /// <summary>
    /// 平行光源用のリソース
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

    /// <summary>
    /// 平行光源データへのポインタ
    /// </summary>
    DirectionalLight* directionalLightData;

    /// <summary>
    /// DirectX共通クラスへのポインタ
    /// </summary>
    DirectXCommon* sDirectXCommon_ = nullptr;

    /// <summary>
    /// カメラオブジェクトへのポインタ
    /// </summary>
    Camera* camera_;

    /// <summary>
    /// ビューポート設定
    /// </summary>
    D3D12_VIEWPORT viewport{};

    /// <summary>
    /// シザー矩形（描画範囲の設定）
    /// </summary>
    D3D12_RECT scissorRect{};
};
