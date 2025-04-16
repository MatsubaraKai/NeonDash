#include "FlowEffectObject.h"
#include <cmath>

void FlowEffectObject::Init() {
    object_.Init();
}

void FlowEffectObject::Update() {
    time_ += 0.016f; // 60FPS想定
    float scaleY = std::sin(time_ * 2.0f) * 0.2f + 1.0f; // Y方向に揺れる感じの演出
    Vector3 scale = object_.GetScale();
    scale.y = scaleY;
    object_.SetScale(scale);

    object_.Update();
}

void FlowEffectObject::Draw(uint32_t texture, Camera* camera) {
    // 将来的に、color_ を使ったカスタムシェーダー描画を差し込めるようにできる
    object_.Draw(texture, camera);
}

void FlowEffectObject::SetPosition(const Vector3& pos) {
    object_.SetPosition(pos);
}

void FlowEffectObject::SetScale(const Vector3& scale) {
    object_.SetScale(scale);
}

void FlowEffectObject::SetRotation(const Vector3& rot) {
    object_.SetRotation(rot);
}

void FlowEffectObject::SetModel(const std::string& filePath) {
    object_.SetModel(filePath);
}

void FlowEffectObject::SetColor(const Vector4& color) {
    color_ = color;
}
