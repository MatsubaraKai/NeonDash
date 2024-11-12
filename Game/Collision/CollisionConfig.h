#pragma once
#include <cstdint>

/// <summary>
/// プレイヤーの衝突属性を表す定数
/// </summary>
const uint32_t kCollisionAttributePlayer = 0b1;

/// <summary>
/// 敵の衝突属性を表す定数
/// </summary>
const uint32_t kCollisionAttributeEnemy = 0b1 << 1;

/// <summary>
/// アイテムの衝突属性を表す定数
/// </summary>
const uint32_t kCollisionAttributeItem = 0b1 << 2;

/// <summary>
/// プレイヤーの弾の衝突属性を表す定数
/// </summary>
const uint32_t kCollisionAttributePlayerBullet = 0b1 << 3;
