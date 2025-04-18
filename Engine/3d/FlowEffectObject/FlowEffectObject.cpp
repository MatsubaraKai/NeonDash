#include "FlowEffectObject.h"
#include <cmath>

void FlowEffectObject::Init() {
    spawnTimer_ = 0.0f;
    ModelManager::GetInstance()->LoadModel("Resources/game/", "star.obj");
    objects_.clear();
}

void FlowEffectObject::Update() {
    spawnTimer_ += 1.0f / 60.0f; // 仮に60FPS前提

    // 一定間隔でオブジェクト生成
    if (spawnTimer_ >= spawnInterval_) {
        SpawnObject();
        spawnTimer_ = 0.0f;
    }

    for (auto it = objects_.begin(); it != objects_.end();) {
        FlowObject& obj = *it;

        // 時間経過
        obj.lifetime += 1.0f / 60.0f;

        // 前方向からカメラ側に移動 & 回転
        Vector3 pos = obj.obj->GetPosition();
        pos.z += 0.1f; // 奥から手前へ
        obj.obj->SetPosition(pos);

        Vector3 rot = obj.obj->GetRotation();
        rot.y += 0.05f; // 回転
        obj.obj->SetRotation(rot);

        // 寿命切れで削除
        if (obj.lifetime >= obj.maxLifetime) {
            it = objects_.erase(it);
        }
        else {
            ++it;
        }
    }
}

void FlowEffectObject::SpawnObject() {
    std::unique_ptr<Object3d> newObj = std::make_unique<Object3d>();
    newObj->Init(); // 必要に応じて初期化処理
    newObj->SetModel("star.obj");
    newObj->SetPosition({ 0.0f, 10.0f, -10.0f }); // 奥からスタート
    newObj->SetRotation({ 0.0f, 0.0f, 0.0f });
    newObj->SetScale({ 10.0f, 10.0f, 10.0f });

    objects_.emplace_back(std::move(newObj), 10.0f); // 5秒で消える

}


void FlowEffectObject::Draw(uint32_t texture, Camera* camera) {
    int i = 0;
    for (auto& obj : objects_) {
        obj.obj->Draw(texture, camera);
        std::string label = "DebugFlowModel" + std::to_string(i++);
        obj.obj->ModelDebug(label.c_str()); // ← インデックスでユニークに
    }
}

void FlowEffectObject::DebugImGui() {
    ImGui::Begin("Flow");

    ImGui::Text("Flowobj count : %d", static_cast<int>(objects_.size()));

    static int selectedIndex = 0;

    if (!objects_.empty()) {
        // Index選択
        ImGui::SliderInt("Selected", &selectedIndex, 0, static_cast<int>(objects_.size()) - 1);

        FlowObject& selected = objects_[selectedIndex];

        Vector3 pos = selected.obj->GetPosition();
        ImGui::Text("Position: X: %.2f, Y: %.2f, Z: %.2f", pos.x, pos.y, pos.z);
        ImGui::Text("Lifetime: %.2f / %.2f", selected.lifetime, selected.maxLifetime);

        std::string label = "DebugModel" + std::to_string(selectedIndex);
        selected.obj->ModelDebug(label.c_str());
    }

    ImGui::End();
}
