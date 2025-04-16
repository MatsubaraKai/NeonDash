#pragma once
#include "Object3d.h"
#include "Vector3.h"

class FlowEffectObject
{
public:
    FlowEffectObject(Object3d* obj);

    void Update();
    void Draw();
    bool IsOutOfRange() const;

    void SetVelocity(const Vector3& vel);
    void SetRotationSpeed(const Vector3& rotSpeed);
    void SetScale(float scale);

private:
    Object3d* obj_;
    Vector3 velocity_;
    Vector3 rotationSpeed_;
    float scale_;
};
