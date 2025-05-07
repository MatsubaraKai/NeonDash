#pragma once
#include "PSOModel.h"
#include "ModelData.h"
#include "MaterialData.h"
#include "vertexData.h"
#include "PSOSkybox.h"
#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include <dxcapi.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <Material.h>
#include <Transform.h>
#include <Camera.h>
#include <WorldTransform.h>
#include "Model.h"
#include "AnimationModel.h"
#include "Object3dCommon.h"
#include <PSOAnimationModel.h>
#include "Skybox.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

/**
* @file Object3d.h
* @brief 3Dオブジェクトを管理するクラス
*/
class Object3d
{
public:
    /// <summary>
    /// 初期化処理を行う
    /// </summary>
    void Init();

    /// <summary>
    /// 更新処理を行う
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理を行う
    /// </summary>
    /// <param name="texture">使用するテクスチャのハンドル</param>
    /// <param name="camera">カメラオブジェクトへのポインタ</param>
    void Draw(uint32_t texture, Camera* camera);

    /// <summary>
    /// リソースを解放する
    /// </summary>
    void Release();

    /// <summary>
    /// モデルのデバッグ情報を表示する
    /// </summary>
    /// <param name="name">デバッグ情報の名前</param>
    void ModelDebug(const char* name);

    /// <summary>
    /// モデルを設定する
    /// </summary>
    /// <param name="model">モデルオブジェクトへのポインタ</param>
    void SetModel(Model* model) { model_ = model; }

    /// <summary>
    /// ファイルパスからモデルを設定する
    /// </summary>
    /// <param name="filePath">モデルファイルのパス</param>
    void SetModel(const std::string& filePath);

    /// <summary>
    /// アニメーションモデルを設定する
    /// </summary>
    /// <param name="filePath">アニメーションモデルファイルのパス</param>
    void SetAnimationModel(const std::string& filePath);

    /// <summary>
    /// スカイボックスを設定する
    /// </summary>
    /// <param name="skybox">スカイボックスオブジェクトへのポインタ</param>
    void SetSkybox(Skybox* skybox) { skybox_ = skybox; }

    /// <summary>
    /// ワールド変換を設定する
    /// </summary>
    /// <param name="worldtransform">ワールド変換データ</param>
    void SetWorldTransform(const WorldTransform& worldtransform) { worldTransform_ = worldtransform; };

    /// <summary>
    /// トランスフォームを設定する
    /// </summary>
    /// <param name="transform">トランスフォームデータ</param>
    void SetTransform(Transform transform);

    /// <summary>
    /// 環境マップ用テクスチャを設定する
    /// </summary>
    /// <param name="maptexture">テクスチャのハンドル</param>
    void SetMapTexture(uint32_t maptexture) { mapTexture_ = maptexture; };

    void SetPosition(const Vector3& pos);
    Vector3 GetPosition() const;

    void SetRotation(const Vector3& rot);
    Vector3 GetRotation() const;

    void SetScale(const Vector3& scale);
    Vector3 GetScale() const;

public: // Getter
    /// <summary>
    /// ワールド変換を取得する
    /// </summary>
    /// <returns>ワールド変換データ</returns>
    WorldTransform GetWorldTransform() { return worldTransform_; }

    /// <summary>
    /// OBJファイルを読み込む
    /// </summary>
    /// <param name="directoryPath">ディレクトリパス</param>
    /// <param name="filename">ファイル名</param>
    /// <returns>モデルデータ</returns>
    ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

    /// <summary>
    /// マテリアルテンプレートファイルを読み込む
    /// </summary>
    /// <param name="directoryPath">ディレクトリパス</param>
    /// <param name="filename">ファイル名</param>
    /// <returns>マテリアルデータ</returns>
    MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

    WorldTransform worldTransform_;
    bool isVisible = true;  // デフォルトで描画する

private:
    Object3dCommon* objectCommon_ = nullptr;
    DirectionalLight* directionalLightData;
    HRESULT hr;
    // RootSignature作成
    Model* model_ = nullptr;
    AnimationModel* animationModel_ = nullptr;
    Skybox* skybox_ = nullptr;
    // データを書き込む
    /*移動用*/
    // WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    TransformationMatrix* wvpData;
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW wvpBufferView{};
    Transform transformUv;

    /*カメラ用*/
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource_;
    CameraForGPU* cameraForGPUData_;

    // 環境マップ用Texture
    uint32_t mapTexture_;

    Vector3 position_;
    Vector3 rotation_;
    Vector3 scale_;
};
