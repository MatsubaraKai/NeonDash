#pragma once
#include <cstdint>

/// <summary>
/// �v���C���[�̏Փˑ�����\���萔
/// </summary>
const uint32_t kCollisionAttributePlayer = 0b1;

/// <summary>
/// �G�̏Փˑ�����\���萔
/// </summary>
const uint32_t kCollisionAttributeEnemy = 0b1 << 1;

/// <summary>
/// �A�C�e���̏Փˑ�����\���萔
/// </summary>
const uint32_t kCollisionAttributeItem = 0b1 << 2;

/// <summary>
/// �v���C���[�̒e�̏Փˑ�����\���萔
/// </summary>
const uint32_t kCollisionAttributePlayerBullet = 0b1 << 3;
