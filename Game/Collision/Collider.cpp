#include "Collider.h"
#include <cmath>

/// <summary>
/// 衝突属性（自分）を設定する関数
/// </summary>
/// <param name="collisionAttribute">設定する衝突属性の値</param>
void Collider::SetCollisonAttribute(uint32_t collisionAttribute) {
    collisionAttribute_ = collisionAttribute;
};

/// <summary>
/// 衝突マスク（相手）を設定する関数
/// </summary>
/// <param name="CollisionMask">設定する衝突マスクの値</param>
void Collider::SetCollisionMask(uint32_t CollisionMask) {
    CollisionMask_ = CollisionMask;
};

/// <summary>
/// 衝突判定を行う関数
/// </summary>
/// <param name="playerPos">プレイヤーの位置ベクトル</param>
/// <param name="objectPos">オブジェクトの位置ベクトル</param>
/// <param name="offsetX">X軸方向のオフセット値</param>
/// <param name="offsetY">Y軸方向のオフセット値</param>
/// <param name="offsetZ">Z軸方向のオフセット値</param>
/// <param name="collisionDistance">衝突と判定する距離の閾値</param>
/// <returns>衝突している場合はtrue、していない場合はfalseを返す</returns>
bool Collider::CheckCollision(const Vector3& playerPos, const Vector3& objectPos, float offsetX, float offsetY, float offsetZ, float collisionDistance) {
    // オブジェクトの位置にオフセットを加算し、プレイヤーの位置との差分を計算
    float dx = (objectPos.x + offsetX) - playerPos.x;
    float dy = (objectPos.y + offsetY) - playerPos.y;
    float dz = (objectPos.z + offsetZ) - playerPos.z;
    // 2点間の距離を計算
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    // 距離が衝突距離より小さい場合は衝突と判定
    return distance < collisionDistance;
}
