#pragma once
#include"Vector3.h"
#include<cstdint>
class Collider {
public:


	// Getter
	// ���a���擾
	int GetRadius() { return radius_; }

	// Setter
	void SetRadius(int radius) { radius_ = radius; }

	virtual void OnCollision() = 0;

	// ���[���h���W���擾
	virtual Vector3 GetWorldPosition() const = 0;

	// �Փˑ����i�����j���擾
	uint32_t GetCollisonAttribute() { return collisionAttribute_; }
	// �Փˑ����i�����j��ݒ�
	void SetCollisonAttribute(uint32_t collisionAttribute);
	// �Փ˃}�X�N�i����j���擾
	uint32_t GetCollisionMask() { return CollisionMask_; }
	// �Փ˃}�X�N�i����j��ݒ�
	void SetCollisionMask(uint32_t CollisionMask);

private:
	// �Փ˔��a
	int radius_ = 2;

	// �Փˑ����i�����j
	uint32_t collisionAttribute_ = 0xFFFFFFFF;
	// �Փ˃}�X�N�i����j
	uint32_t CollisionMask_ = 0xffffffff;

};
