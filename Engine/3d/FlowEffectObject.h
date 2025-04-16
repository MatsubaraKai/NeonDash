#pragma once
#include "Object3d.h"

class FlowEffectObject {
public:
    void Init();
    void Update();
    void Draw(uint32_t texture, Camera* camera);
    void SetPosition(const Vector3& pos);
    void SetScale(const Vector3& scale);
    void SetRotation(const Vector3& rot);
    void SetModel(const std::string& filePath);
    void SetColor(const Vector4& color);

private:
    Object3d object_; // 中身は普通のObject3d
    Vector4 color_ = { 1, 1, 1, 1 }; // 色
    float time_ = 0.0f;           // 時間（アニメーション制御用）
};
