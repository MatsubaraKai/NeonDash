// FireworkParticle.cpp

// 既存のインクルードファイル
#include "FireworkParticle.h"
#include "PSOFireworkParticle.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ImGui.h"
#include <random>
#include <numbers>
#include <cmath>

// 既存の using 宣言
using std::sinf;
using std::cosf;

FireworkParticle::FireworkParticle()
    : sWinAPI(nullptr),
    sDirectXCommon(nullptr),
    materialData_(nullptr),
    instancingData_(nullptr),
    directionalLightData_(nullptr),
    numInstance_(0) {
    // 初回の時間を記録
    previousTime_ = std::chrono::steady_clock::now();
    // 乱数エンジンの初期化
    randomEngine_.seed(std::random_device{}());
}
/**
 * @brief デストラクタ
 */
FireworkParticle::~FireworkParticle() {}

float FireworkParticle::GetDeltaTime() {
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - previousTime_;
    previousTime_ = currentTime;
    return elapsedTime.count();
}
/**
 * @brief パーティクルシステムを初期化します。
 * @param emitter 初期エミッター設定
 */
void FireworkParticle::Initialize(FireEmitter emitter) {
    sWinAPI = WinAPI::GetInstance();
    sDirectXCommon = DirectXCommon::GetInstance();

    // Sprite用の頂点リソースを作成
    vertexResource_ = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(VertexData) * 4);

    // 頂点バッファビューの設定
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    // 頂点データをマッピング
    VertexData* vertexData = nullptr;
    HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    if (FAILED(hr)) {
        // エラーメッセージを出力し、プログラムを終了する
        OutputDebugStringA("Failed to map vertex resource.\n");
        assert(false);
        return;
    }

    // 四角形の頂点設定（ビルボード用）
    vertexData[0].position = { -1.0f, -1.0f, 0.0f, 1.0f }; // 左下
    vertexData[0].texcorrd = { 0.0f, 1.0f };
    vertexData[1].position = { -1.0f, 1.0f, 0.0f, 1.0f };  // 左上
    vertexData[1].texcorrd = { 0.0f, 0.0f };
    vertexData[2].position = { 1.0f, -1.0f, 0.0f, 1.0f };  // 右下
    vertexData[2].texcorrd = { 1.0f, 1.0f };
    vertexData[3].position = { 1.0f, 1.0f, 0.0f, 1.0f };   // 右上
    vertexData[3].texcorrd = { 1.0f, 0.0f };

    // マテリアルリソースの作成
    materialResource_ = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(Material));
    HRESULT hr2 = materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    if (FAILED(hr2)) {
        OutputDebugStringA("Failed to map material resource.\n");
        assert(false);
        return;
    }
    materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    materialData_->uvTransform = MakeIdentity4x4();
    materialData_->enableLighting = false;

    // インスタンシング用リソースの作成
    transformationResource_ = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(FireworkParticleForGPU) * kNumMaxFireworkInstances);
    transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
    for (uint32_t index = 0; index < kNumMaxFireworkInstances; ++index) {
        instancingData_[index].WVP = MakeIdentity4x4();
        instancingData_[index].World = MakeIdentity4x4();
        instancingData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    // インデックスバッファの作成
    indexResource_ = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(uint32_t) * 6);
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    // インデックスデータの設定
    uint32_t* indexData = nullptr;
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    indexData[3] = 1;
    indexData[4] = 3;
    indexData[5] = 2;

    // SRVの設定
    SRVIndex_ = SRVManager::Allocate();
    SRVManager::CreateSRVforStructuredBuffer(SRVIndex_, transformationResource_.Get(), kNumMaxFireworkInstances, sizeof(FireworkParticleForGPU));
    instancingSrvHandleGPU_ = SRVManager::GetGPUDescriptorHandle(SRVIndex_);

    // ライトデータの設定
    directionalLightResource_ = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(DirectionalLight));
    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
    directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };
    directionalLightData_->intensity = 1.0f;

    // エミッター設定
    emitter_ = emitter;
    emitter_.count = 6;
    emitter_.frequency = 0.02f; // 0.02秒ごとに発生
    emitter_.frequencyTime = 0.0f; // 発生頻度用の時刻、0で初期化
}

/**
 * @brief 花火を発射します。
 * @param position 発射位置
 */
void FireworkParticle::LaunchFirework(const Vector3& position) {
    Particle launchParticle;
    launchParticle.phase = FireworkPhase::Launch;
    launchParticle.transform.translate = position;
    launchParticle.velocity = { 0.0f, 5.0f, 0.0f }; // 上方向への速度
    launchParticle.lifeTime = 2.0f; // 打ち上げフェーズの寿命
    launchParticle.currentTime = 0.0f;
    launchParticle.peakHeight = position.y + 10.0f; // 花火の頂点高度
    launchParticle.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色の玉

    particles_.push_back(launchParticle);
}

/**
 * @brief パーティクルを更新します。
 * @param deltaTime 前フレームからの経過時間
 * @param camera カメラ情報
 */
void FireworkParticle::Update(Camera* camera) {
    float deltaTime = GetDeltaTime();

    // エミッターの時間更新と発生
    emitter_.frequencyTime += deltaTime;
    if (emitter_.frequency <= emitter_.frequencyTime) {
        Vector3 launchPosition = {
            GetRandomFloat(-10.0f, 10.0f),
            0.0f,
            GetRandomFloat(-10.0f, 10.0f)
        };
        LaunchFirework(launchPosition);
        emitter_.frequencyTime -= emitter_.frequency;
    }
    // インスタンス数リセット
    numInstance_ = 0;

    // パーティクルの更新
    for (auto it = particles_.begin(); it != particles_.end();) {
        Particle& particle = *it;
        particle.currentTime += deltaTime;

        switch (particle.phase) {
        case FireworkPhase::Launch:
            // 上昇処理
            particle.transform.translate.y += particle.velocity.y * deltaTime;
            // 頂点高度に達したら爆発フェーズに移行
            if (particle.transform.translate.y >= particle.peakHeight) {
                particle.phase = FireworkPhase::Explosion;
                particle.currentTime = 0.0f;
                EmitExplosionParticles(particle.transform.translate);
                it = particles_.erase(it); // 打ち上げパーティクルは削除
                continue;
            }
            break;

        case FireworkPhase::Explosion:
            // 爆発パーティクルの更新は EmitExplosionParticles 内で生成済み
            // このフェーズでは特に更新は不要
            break;

        case FireworkPhase::Fading:
            // 消散処理
            particle.color.w -= deltaTime * 0.5f; // 徐々に透明に
            if (particle.color.w <= 0.0f) {
                it = particles_.erase(it); // パーティクルを削除
                continue;
            }
            break;
        }

        // 描画用のインスタンスデータを設定
        if (numInstance_ < kNumMaxFireworkInstances) {
            // ビルボード行列の計算
            Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
            Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera->GetCameraMatrix());
            billboardMatrix.m[3][0] = 0.0f;
            billboardMatrix.m[3][1] = 0.0f;
            billboardMatrix.m[3][2] = 0.0f;

            // ワールド行列の計算
            Matrix4x4 worldMatrix = Multiply(MakeScaleMatrix(particle.transform.scale), Multiply(billboardMatrix, MakeTranslateMatrix(particle.transform.translate)));
            Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));

            instancingData_[numInstance_].WVP = worldViewProjectionMatrix;
            instancingData_[numInstance_].World = worldMatrix;
            instancingData_[numInstance_].color = particle.color;

            numInstance_++;
        }

        ++it;
    }
}
float FireworkParticle::GetRandomFloat(float minValue, float maxValue) {
    std::uniform_real_distribution<float> dist(minValue, maxValue);
    return dist(randomEngine_);
}
/**
 * @brief 爆発パーティクルを生成します。
 * @param position 爆発位置
 */
void FireworkParticle::EmitExplosionParticles(const Vector3& position) {
    // ランダムな方向と色を生成する際に、クラスメンバの randomEngine_ を使用
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> distSpeed(2.0f, 5.0f);
    std::uniform_real_distribution<float> distColor(0.0f, 1.0f);

    const int numParticles = 100; // 爆発時のパーティクル数
    for (int i = 0; i < numParticles; ++i) {
        Particle explosionParticle;
        // 既存のパーティクル設定
        explosionParticle.color = Vector4(distColor(randomEngine_), distColor(randomEngine_), distColor(randomEngine_), 1.0f);
        float theta = distAngle(randomEngine_);
        float phi = distAngle(randomEngine_);
        float speed = distSpeed(randomEngine_);

        explosionParticle.velocity = {
            speed * sinf(theta) * cosf(phi),
            speed * sinf(theta) * sinf(phi),
            speed * cosf(theta)
        };

        particles_.push_back(explosionParticle);
    }
}

/**
 * @brief パーティクルを描画します。
 * @param texture テクスチャハンドル
 * @param camera カメラ情報
 */
void FireworkParticle::Draw(uint32_t texture, Camera* camera) {
    if (numInstance_ == 0) return; // 描画するインスタンスがない場合はスキップ

    // PSOの取得と設定
    pso_ = PSOFireworkParticle::GetInstance();
    sDirectXCommon->GetCommandList()->SetGraphicsRootSignature(pso_->GetProperty().rootSignature.Get());
    sDirectXCommon->GetCommandList()->SetPipelineState(pso_->GetProperty().graphicsPipelineState.Get());

    // バッファ設定
    sDirectXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
    sDirectXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
    sDirectXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 定数バッファの設定
    sDirectXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

    // インスタンシングデータの設定
    sDirectXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

    // テクスチャの設定
    sDirectXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetGPUDescriptorHandle(texture));

    // ライトの設定
    sDirectXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

    // 描画コール
    sDirectXCommon->GetCommandList()->DrawIndexedInstanced(6, numInstance_, 0, 0, 0);
}

/**
 * @brief デバッグ用のインターフェースを表示します。
 * @param name デバッグウィンドウの名前
 * @param worldtransform ワールド変換
 */
void FireworkParticle::Particledebug(const char* name, WorldTransform& worldtransform) {
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.7f, 0.2f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.3f, 0.1f, 0.5f));
    ImGui::Begin("Firework Particle");

    if (ImGui::TreeNode(name)) {
        float translate[3] = { worldtransform.translation_.x, worldtransform.translation_.y, worldtransform.translation_.z };
        ImGui::DragFloat3("Transform", translate, 0.01f);
        ImGui::TreePop();
        worldtransform.translation_ = { translate[0], translate[1], translate[2] };
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}
