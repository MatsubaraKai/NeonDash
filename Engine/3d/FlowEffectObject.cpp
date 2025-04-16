#include "FlowEffectObject.h"

FlowEffectObject::FlowEffectObject(Object3d* obj)
    : obj_(obj), velocity_(0, 0, -0.2f), rotationSpeed_(0, 0, 0), scale_(1.0f) {}

void FlowEffectObject::Update()
{
    Vector3 pos = obj_->GetPosition();
    pos += velocity_;
    obj_->SetPosition(pos);

    Vector3 rot = obj_->GetRotation();
    rot += rotationSpeed_;
    obj_->SetRotation(rot);
}

void FlowEffectObject::Draw()
{
    obj_->Draw();
}

bool FlowEffectObject::IsOutOfRange() const
{
    return obj_->GetPosition().z < -10.0f;
}

void FlowEffectObject::SetVelocity(const Vector3& vel)
{
    velocity_ = vel;
}

void FlowEffectObject::SetRotationSpeed(const Vector3& rotSpeed)
{
    rotationSpeed_ = rotSpeed;
}

void FlowEffectObject::SetScale(float scale)
{
    scale_ = scale;
    obj_->SetScale({ scale, scale, scale });
}

