#pragma once
#include "Collider.h"
#include <list>

/// <summary>
/// @file CollisionManager.h
/// @brief 衝突判定を管理するクラスの宣言
/// </summary>

class GameScene;

/// <summary>
/// 複数のコライダー間の衝突判定を管理するクラス
/// </summary>
class CollisionManager {
public:
    /// <summary>
    /// 2つのコライダー間の衝突判定を行う関数
    /// </summary>
    /// <param name="colliderA">コライダーA</param>
    /// <param name="colliderB">コライダーB</param>
    void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

    /// <summary>
    /// 登録されている全てのコライダー間で衝突判定を行う関数
    /// </summary>
    void CheckAllCollision();

    /// <summary>
    /// コライダーをリストに追加する関数
    /// </summary>
    /// <param name="collider">追加するコライダーのポインタ</param>
    void PushClider(Collider* collider);

    /// <summary>
    /// ゲームシーンを設定する関数
    /// </summary>
    /// <param name="gamescene">設定するゲームシーンのポインタ</param>
    void SetGameScene(GameScene* gamescene) { gameScene_ = gamescene; }

private:
    /// <summary>
    /// ゲームシーンのポインタ
    /// </summary>
    GameScene* gameScene_ = nullptr;

    /// <summary>
    /// 登録されたコライダーのリスト
    /// </summary>
    std::list<Collider*> colliders_;
};
