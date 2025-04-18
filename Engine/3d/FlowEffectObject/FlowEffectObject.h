#pragma once
#include "Object3d.h"
#include <vector>
#include <memory>
#include "ModelManager.h"

struct FlowObject {
    std::unique_ptr<Object3d> obj;
    float lifetime = 0.0f;
    float maxLifetime = 100.0f;

    Vector3 rotationSpeed = { 0.0f, 0.05f, 0.0f };
    Vector3 baseScale = { 1.0f, 1.0f, 1.0f };
    FlowObject(std::unique_ptr<Object3d> o, float maxLife)
        : obj(std::move(o)), maxLifetime(maxLife), lifetime(0.0f) {}
};


class FlowEffectObject {
public:
    void Init();
    void Update();
    void Draw(uint32_t texture, Camera* camera);
    void DebugImGui();
private:
    void SpawnObject(); // 新規オブジェクト生成
    float life = 0.0f;
    std::vector<FlowObject> objects_;
    float spawnTimer_ = 0.0f;
    float spawnInterval_ = 0.5f; // 0.5秒ごとに生成
    int selectedIndex = 0;
};
