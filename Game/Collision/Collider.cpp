#include "Collider.h"

// �Փˑ����i�����j��ݒ�
void Collider::SetCollisonAttribute(uint32_t collisionAttribute) {
	collisionAttribute_ = collisionAttribute;
};
// �Փ˃}�X�N�i����j��ݒ�
void Collider::SetCollisionMask(uint32_t CollisionMask) {
	CollisionMask_ = CollisionMask;
};