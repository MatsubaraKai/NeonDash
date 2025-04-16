#include "FlowEffectManager.h"
#include <cstdlib> // rand

void FlowEffectManager::Update()
{
    for (auto& effect : effects_)
    {
        effect.Update();
    }

    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(),
            [](const FlowEffectObject& e) { return e.IsOutOfRange(); }),
        effects_.end());
}

void FlowEffectManager::Draw()
{
    for (auto& effect : effects_)
    {
        effect.Draw();
    }
}

void FlowEffectManager::CreateEffect()
{
    Object3d* obj = Object3d::Create(); // Object3d¶¬iŠÂ‹«‚É‰ž‚¶‚Ä•ÏXj

    Vector3 pos = {
        RandomFloat(-5.0f, 5.0f),
        RandomFloat(-3.0f, 3.0f),
        20.0f
    };
    obj->SetPosition(pos);

    FlowEffectObject effect(obj);
    effect.SetVelocity({ 0, 0, -RandomFloat(0.1f, 0.3f) });
    effect.SetRotationSpeed({
        RandomFloat(-1.0f, 1.0f),
        RandomFloat(-1.0f, 1.0f),
        RandomFloat(-1.0f, 1.0f)
        });

    float scale = RandomFloat(0.5f, 2.0f);
    effect.SetScale(scale);

    effects_.push_back(effect);
}

float FlowEffectManager::RandomFloat(float min, float max)
{
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}
