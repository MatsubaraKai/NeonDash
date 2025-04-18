#pragma once
#include "Object3d.h"
#include <vector>
#include <memory>

struct FlowObject {
    std::unique_ptr<Object3d> obj;
    float lifetime = 0.0f; // 経過時間
    float maxLifetime = 5.0f; // 消えるまでの時間

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

    std::vector<FlowObject> objects_;
    float spawnTimer_ = 0.0f;
    float spawnInterval_ = 0.5f; // 0.5秒ごとに生成
    int selectedIndex = 0;
};
