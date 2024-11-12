#include "CollisionManager.h"
#include "STAGE1.h"
//#include "Enemy.h"
//#include "Player.h"
//#include "AxisIndicator.h"

/// <summary>
/// 全てのコリジョンをチェックする関数
/// </summary>
void CollisionManager::CheckAllCollision() {

    //// 自弾リストの取得（コメントアウト）
    //const std::list<PlayerBullet*>& playerBullets = player_->Getbullet();

    //// 敵弾リストの取得（コメントアウト）
    //const std::list<EnemyBullet*>& enemyBullets = enemy_->Getbullet();

    //// コライダーをリストに登録（コメントアウト）
    //std::list<Collider*> colliders_;
    //colliders_.push_back(player_);
    //colliders_.push_back(enemy_);

    //for (PlayerBullet* bullet : playerBullets) {
    //    colliders_.push_back(bullet);
    //}
    //for (EnemyBullet* bullet : enemyBullets) {
    //    colliders_.push_back(bullet);
    //}

    // コライダーのペアを総当たりでチェック
    std::list<Collider*>::iterator itrA = colliders_.begin();
    for (; itrA != colliders_.end(); ++itrA) {

        // イテレータBはイテレータAの次の要素から開始（重複判定を回避）
        std::list<Collider*>::iterator itrB = itrA;
        itrB++;
        for (; itrB != colliders_.end(); ++itrB) {

            // ペアの当たり判定を行う
            CheckCollisionPair(*itrA, *itrB);
        }
    }
}

/// <summary>
/// コライダーをリストに追加する関数
/// </summary>
/// <param name="collider">追加するコライダーのポインタ</param>
void CollisionManager::PushClider(Collider* collider)
{
    colliders_.push_back(collider);
}

/// <summary>
/// 2つのコライダー間の衝突判定を行う関数
/// </summary>
/// <param name="colliderA">コライダーA</param>
/// <param name="colliderB">コライダーB</param>
void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
    // 判定対象AとBの座標と半径を取得
    Vector3 posA = colliderA->GetWorldPosition();
    int radiusA = colliderA->GetRadius();

    Vector3 posB = colliderB->GetWorldPosition();
    int radiusB = colliderB->GetRadius();

    // 2点間の距離の二乗を計算
    float p2b = (posB.x - posA.x) * (posB.x - posA.x) +
        (posB.y - posA.y) * (posB.y - posA.y) +
        (posB.z - posA.z) * (posB.z - posA.z);

    // 半径の和の二乗を計算
    int r2r = (radiusA + radiusB) * (radiusA + radiusB);

    /* 衝突フィルタリング（コメントアウト）
    if (((colliderA->GetCollisonAttribute() & colliderB->GetCollisionMask()) != 0) ||
        ((colliderB->GetCollisonAttribute() & colliderA->GetCollisionMask()) != 0)) {
        return;
    };
    */

    // 衝突判定
    if (p2b <= r2r) {
        // 衝突フィルタリング
        if (colliderA->GetCollisonAttribute() != colliderB->GetCollisionMask() ||
            colliderB->GetCollisonAttribute() != colliderA->GetCollisionMask()) {
            return;
        };
        // コライダーAの衝突時コールバックを呼び出す
        colliderA->OnCollision();
        // コライダーBの衝突時コールバックを呼び出す
        colliderB->OnCollision();
    }
};
