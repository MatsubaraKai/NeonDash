#pragma once
#include "Vector3.h"
#include <cstdint>

/// <summary>
/// @file Collider.h
/// @brief 衝突判定を行うコライダーの基底クラスの宣言
/// </summary>

/// <summary>
/// 衝突判定に必要な情報と機能を提供するコライダークラス。
/// </summary>
class Collider {
public:

    /// <summary>
    /// 衝突半径を取得する関数
    /// </summary>
    /// <returns>衝突半径の値</returns>
    int GetRadius() { return radius_; }

    /// <summary>
    /// 衝突半径を設定する関数
    /// </summary>
    /// <param name="radius">設定する衝突半径の値</param>
    void SetRadius(int radius) { radius_ = radius; }

    /// <summary>
    /// 衝突時に呼ばれる仮想関数（純粋仮想関数）
    /// </summary>
    virtual void OnCollision() = 0;

    /// <summary>
    /// ワールド座標を取得する純粋仮想関数
    /// </summary>
    /// <returns>ワールド座標（Vector3）</returns>
    virtual Vector3 GetWorldPosition() const = 0;

    /// <summary>
    /// 衝突属性（自分）を取得する関数
    /// </summary>
    /// <returns>衝突属性の値（uint32_t）</returns>
    uint32_t GetCollisonAttribute() { return collisionAttribute_; }

    /// <summary>
    /// 衝突属性（自分）を設定する関数
    /// </summary>
    /// <param name="collisionAttribute">設定する衝突属性の値</param>
    void SetCollisonAttribute(uint32_t collisionAttribute);

    /// <summary>
    /// 衝突マスク（相手）を取得する関数
    /// </summary>
    /// <returns>衝突マスクの値（uint32_t）</returns>
    uint32_t GetCollisionMask() { return CollisionMask_; }

    /// <summary>
    /// 衝突マスク（相手）を設定する関数
    /// </summary>
    /// <param name="CollisionMask">設定する衝突マスクの値</param>
    void SetCollisionMask(uint32_t CollisionMask);

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
    bool CheckCollision(const Vector3& playerPos, const Vector3& objectPos, float offsetX, float offsetY, float offsetZ, float collisionDistance);

private:
    /// <summary>
    /// 衝突半径
    /// </summary>
    int radius_ = 2;

    /// <summary>
    /// 衝突属性（自分）
    /// </summary>
    uint32_t collisionAttribute_ = 0xFFFFFFFF;

    /// <summary>
    /// 衝突マスク（相手）
    /// </summary>
    uint32_t CollisionMask_ = 0xffffffff;

};
