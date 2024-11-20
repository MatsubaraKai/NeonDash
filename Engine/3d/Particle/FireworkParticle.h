// FireworkParticle.h
#pragma once

#include <list>
#include <random>
#include <wrl.h> // 追加
using Microsoft::WRL::ComPtr; // 追加
#include "PSOFireworkParticle.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "SRVManager.h"
#include "DirectXCommon.h"
#include "WinAPI.h"
#include "mathFunction.h"
#include <chrono>
// 以降、ComPtr を使用している箇所はそのままで大丈夫です

#include "WorldTransform.h"

// 定数定義
constexpr uint32_t kNumMaxFireworkInstances = 1000;

// パーティクルのフェーズを定義
enum class FireworkPhase {
    Launch,
    Explosion,
    Fading
};

// GPUで使用するパーティクルデータ構造
struct FireworkParticleForGPU {
    Matrix4x4 WVP;    // World-View-Projection Matrix
    Matrix4x4 World;  // World Matrix
    Vector4 color;    // 色 (RGBA)
};

struct FireEmitter {
    int count;
    float frequency;
    float frequencyTime;
    Transform transform;
};

class FireworkParticle {
public:

    // パーティクル構造体
    struct Particle {
        Transform transform;
        Vector3 velocity;
        Vector4 color;
        float lifeTime;
        float currentTime;
        FireworkPhase phase;
        float peakHeight;
    };
    FireworkParticle();
    ~FireworkParticle();

    /**
     * @brief パーティクルシステムを初期化します。
     * @param emitter 初期エミッター設定
     */
    void Initialize(FireEmitter emitter);

    /**
     * @brief 花火を発射します。
     * @param position 発射位置
     */
    void LaunchFirework(const Vector3& position);

    /**
     * @brief パーティクルを更新します。
     * @param deltaTime 前フレームからの経過時間
     * @param camera カメラ情報
     */
    void Update(Camera* camera);


    /**
     * @brief パーティクルを描画します。
     * @param texture テクスチャハンドル
     */
    void Draw(uint32_t texture, Camera* camera);

    /**
     * @brief デバッグ用のインターフェースを表示します。
     * @param name デバッグウィンドウの名前
     * @param worldtransform ワールド変換
     */
    void Particledebug(const char* name, WorldTransform& worldtransform);
    float GetRandomFloat(float minValue, float maxValue);

private:
    /**
     * @brief 新しい爆発パーティクルを生成します。
     * @param position 爆発位置
     */
    void EmitExplosionParticles(const Vector3& position);
    float GetDeltaTime();

    // パーティクルリスト
    std::list<Particle> particles_;
    std::chrono::steady_clock::time_point previousTime_;
    std::mt19937 randomEngine_;
    // DirectX関連
    WinAPI* sWinAPI;
    DirectXCommon* sDirectXCommon;
    ComPtr<ID3D12Resource> vertexResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    FireworkParticleForGPU* particleData_;
    ComPtr<ID3D12Resource> materialResource_;
    Material* materialData_;
    ComPtr<ID3D12Resource> transformationResource_;
    FireworkParticleForGPU* instancingData_;
    ComPtr<ID3D12Resource> indexResource_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;
    PSOFireworkParticle* pso_;
    uint32_t SRVIndex_;
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

    // ライトデータ
    DirectionalLight* directionalLightData_;
    ComPtr<ID3D12Resource> directionalLightResource_;

    // エミッター設定
    FireEmitter emitter_;

    // インスタンス数カウンタ
    uint32_t numInstance_;
};
