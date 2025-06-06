﻿#include "FlowEffectObject.h"
#include <cmath>
#include <random>

float RandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

void FlowEffectObject::Init() {
    spawnTimer_ = 0.0f;
    ModelManager::GetInstance()->LoadModel("Resources/game/", "cube.obj");
    objects_.clear();

}

void FlowEffectObject::Update() {
    spawnTimer_ += 1.0f / 10.0f; // 仮に30FPS前提

    // 一定間隔でオブジェクト生成
    if (spawnTimer_ >= spawnInterval_) {
        SpawnObject();
        spawnTimer_ = 0.0f;
    }

    for (auto it = objects_.begin(); it != objects_.end();) {
        FlowObject& obj = *it;
        obj.lifetime += 1.0f / 60.0f;

        WorldTransform wt = obj.obj->GetWorldTransform();

        obj.rotationSpeed = { 0.01f, RandomFloat(0.01f, 0.1f), 0.01f };

        // Z方向スピードもランダム（近づく速さ）
        //obj.moveSpeed = RandomFloat(0.1f, 3.0f);

        obj.baseScale = wt.scale_; // 忘れず baseScale も代入

        wt.translation_.z -= 2.5f;

        wt.rotation_ += obj.rotationSpeed; // ランダムな回転速度で回転

        wt.UpdateMatrix();
        obj.obj->SetWorldTransform(wt);

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
    newObj->Init();
    newObj->SetModel("cube.obj");

    // ランダム値
    float randX = RandomFloat(-700.0f, 700.0f); // X方向にランダム位置
    float randY = RandomFloat(-700.0f, 700.0f); // Y方向にランダム位置
    float randZ = RandomFloat(1500.0f, 1600.0f); // Z方向（遠くから）

    float randScale = RandomFloat(1.0f, 5.0f); // 大きさ

    float randRotX = RandomFloat(0.0f, 3.14f); // 初期回転
    float randRotY = RandomFloat(0.0f, 3.14f);
    float randRotZ = RandomFloat(0.0f, 3.14f);

    // ランダム回転速度を保存したいので、FlowObjectに新しいメンバを追加
    WorldTransform wt;
    wt.translation_ = { randX, randY, randZ };
    wt.rotation_ = { randRotX, randRotY, randRotZ };
    wt.scale_ = { randScale, randScale, randScale };

    wt.UpdateMatrix();
    newObj->SetWorldTransform(wt);
    FlowObject flowObj(std::move(newObj), 12.0f);
    flowObj.rotationSpeed = { 0.01f, RandomFloat(0.01f, 0.1f), 0.01f }; // Y軸中心に

    objects_.emplace_back(std::move(flowObj));
}


void FlowEffectObject::Draw(uint32_t texture, Camera* camera) {
    int i = 0;
    for (auto& obj : objects_) {
        obj.obj->Draw(texture, camera);
#ifdef _DEBUG
        std::string label = "DebugFlowModel" + std::to_string(i++);
        obj.obj->ModelDebug(label.c_str()); // ← インデックスでユニークに
#endif

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
        ImGui::Text("Lifetime: %.2f / %.2f", selected.lifetime, selected.maxLifetime);

        std::string label = "DebugModel" + std::to_string(selectedIndex);
        selected.obj->ModelDebug(label.c_str());
    }

    ImGui::End();
}
