#pragma once
#include <vector>
#include "FlowEffectObject.h"

class FlowEffectManager
{
public:
    void Update();
    void Draw();
    void CreateEffect(); // �Ăяo�����Ƃ�1���

private:
    std::vector<FlowEffectObject> effects_;
    float RandomFloat(float min, float max);
};
