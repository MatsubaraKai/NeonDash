#pragma once
#include "PSOParticle.h"

#include <DirectXTex.h>
#include <string>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxcapi.h>
#include <random>

#include "function.h"
#include <wrl.h>
#include "Transform.h"
#include "VertexData.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "DirectionLight.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "imgui.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

/**
* @file Particle.h
* @brief Particle class
*/
class WinAPI;
class DirectXCommon;
class Camera;
class Mesh;

struct ParticleForGPU {
    Matrix4x4 WVP;
    Matrix4x4 World;
    Vector4 color;
};

struct Emitter {
    Transform transform; //!< エミッタのTransform
    uint32_t count; //!< 発生数
    float frequency; //!< 発生頻度
    float frequencyTime; //!< 頻度用時刻
};

struct RandRangePro {
    Vector2 rangeX;
    Vector2 rangeY;
    Vector2 rangeZ;
};

class Particle
{
public:
    struct ParticlePro { // プロパティ
        Transform transform;
        Vector3 velocity;
        Vector4 color;
        float lifeTime;
        float currentTime;
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Particle();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Particle();

    /// <summary>
    /// パーティクルの初期化を行う
    /// </summary>
    /// <param name="emitter">エミッタ情報</param>
    void Initialize(Emitter emitter);

    /// <summary>
    /// パーティクルの描画を行う
    /// </summary>
    /// <param name="emitter">エミッタ情報</param>
    /// <param name="worldTransformPa">ワールド変換パラメータ</param>
    /// <param name="texture">使用するテクスチャハンドル</param>
    /// <param name="camera">カメラオブジェクトへのポインタ</param>
    /// <param name="randRange">ランダム範囲プロパティ</param>
    /// <param name="scaleAddFlag">スケールを加算するかのフラグ</param>
    void Draw(Emitter emitter, const Vector3& worldTransformPa, uint32_t texture, Camera* camera,
        const RandRangePro& randRange, bool scaleAddFlag, float minLifetime, float maxLifetime);

    /// <summary>
    /// リソースの解放を行う
    /// </summary>
    void Release();

    /// <summary>
    /// テクスチャマネージャーを設定する
    /// </summary>
    /// <param name="textureManager">テクスチャマネージャーのポインタ</param>
    void SetTextureManager(TextureManager* textureManager) {
        textureManager_ = textureManager;
    }

    /// <summary>
    /// 新しいパーティクルを生成する
    /// </summary>
    /// <param name="randomEngine">乱数エンジン</param>
    /// <param name="scale">スケールベクトル</param>
    /// <param name="translate">平行移動ベクトル</param>
    /// <param name="randRange">ランダム範囲プロパティ</param>
    /// <returns>生成されたパーティクルプロパティ</returns>
    ParticlePro MakeNewParticle(std::mt19937& randomEngine, const Vector3& scale, const Vector3& translate, const RandRangePro& randRange);

    /// <summary>
    /// パーティクルを発生させる
    /// </summary>
    /// <param name="emitter">エミッタ情報</param>
    /// <param name="randEngine">乱数エンジン</param>
    /// <param name="worldTransformPa">ワールド変換パラメータ</param>
    /// <param name="randRange">ランダム範囲プロパティ</param>
    /// <returns>生成されたパーティクルのリスト</returns>
    std::list<ParticlePro> Emission(const Emitter& emitter, std::mt19937& randEngine, const Vector3& worldTransformPa, const RandRangePro& randRange);

    /// <summary>
    /// 頂点バッファビューを作成する
    /// </summary>
    /// <returns>作成された頂点バッファビュー</returns>
    D3D12_VERTEX_BUFFER_VIEW CreateBufferView();

    /// <summary>
    /// パーティクルのデバッグ情報を表示する
    /// </summary>
    /// <param name="name">デバッグ名</param>
    /// <param name="worldtransform">ワールド変換情報</param>
    void Particledebug(const char* name, WorldTransform& worldtransform);
    // Δtを定義。とりあえず60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておくとなお良い
    const float kDeltaTime = 1.0f / 120.0f;
    std::list<ParticlePro> particles_;

private:
    const static uint32_t kNumMaxInstance = 10000; // インスタンス数

    // Instancing用のTransformMatrixリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResorce = nullptr;

    PSOParticle* pso_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite_ = nullptr;
    WinAPI* sWinAPI;
    DirectXCommon* sDirectXCommon;
    Mesh* mesh_;
    TextureManager* textureManager_ = nullptr;

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};

    VertexData* vertexDataSprite_ = nullptr;

    // Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResouceSprite;

    D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

    Particle* particle = nullptr;

    uint32_t* indexDataSprite;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite;
    D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

    // 実際に頂点リソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW materialBufferView{};

    // 頂点リソースにデータを書き込む
    Material* materialData;

    std::list<Transform> transforms_;
    ParticleForGPU* instancingData = nullptr;

    // 平行光源用
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

    // データを書き込む
    DirectionalLight* directionalLightData;
    Transform transformUv;

    uint32_t SRVIndex_;

   

    Emitter emitter_{};
    RandRangePro randRange_;
};
