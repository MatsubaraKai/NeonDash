#pragma once
#include <vector>
#include "FlowEffectObject.h"

class FlowEffectManager
{
public:
    void Update();
    void Draw();
    void CreateEffect(); // ŒÄ‚Ño‚µ‚²‚Æ‚É1‚Âì‚é

private:
    std::vector<FlowEffectObject> effects_;
    float RandomFloat(float min, float max);
};
