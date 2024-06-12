#include "CollisionManager.h"
#include "GameScene.h"
//#include "Enemy.h"
//#include "Player.h"
//#include "AxisIndicator.h"

void CollisionManager::CheckAllCollision() {

	//// ���e���X�g�̎擾
	//const std::list<PlayerBullet*>& playerBullets = player_->Getbullet();

	//// �G�e���X�g�̎擾
	//const std::list<EnemyBullet*>& enemyBullets = enemy_->Getbullet();

	//std::list<Collider*> colliders_;
	//// �R���C�_�[�����X�g�ɓo�^
	//colliders_.push_back(player_);
	//colliders_.push_back(enemy_);

	//for (PlayerBullet* bullet : playerBullets) {
	//	colliders_.push_back(bullet);
	//}
	//for (EnemyBullet* bullet : enemyBullets) {
	//	colliders_.push_back(bullet);
	//}

	// std::list<Collider*> colliders;
	//  ���X�g���̃y�A�𑍓�����
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {

		// �C�e���[�^B�̓C�e���[�^�\A�̎��̗v�f����񂷁i�d�����������j
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {

			// �y�A�̓����蔻��
			CheckCollisionPair(*itrA, *itrB);
		}
	}
}

void CollisionManager::PushClider(Collider* collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// ����Ώ�A��B�̍��W
	Vector3 posA, posB;
	int radiusA, radiusB;

	// colliderA�̍��W
	posA = colliderA->GetWorldPosition();
	radiusA = colliderA->GetRadius();

	// colliderB�̍��W
	posB = colliderB->GetWorldPosition();
	radiusB = colliderB->GetRadius();
	// �e�ƒe�̍l��������
	// �Փ˃t�B���^�����O

	float p2b = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) +
		(posB.z - posA.z) * (posB.z - posA.z);
	int r2r = (radiusA + radiusB) * (radiusA + radiusB);
	/*  if (((colliderA->GetCollisonAttribute() & colliderB->GetCollisionMask())!=0) ||
		  ((colliderB->GetCollisonAttribute() & colliderA->GetCollisionMask()))!=0) {
	  return;
	  };*/

	if (p2b <= r2r) {
		if (colliderA->GetCollisonAttribute() != colliderB->GetCollisionMask() ||
			colliderB->GetCollisonAttribute() != colliderA->GetCollisionMask()) {
			return;
		};
		// �R���C�_�[A�̏Փˎ��R�[���o�b�N���Ăяo��
		colliderA->OnCollision();
		// �R���C�_�[B�̏Փˎ��R�[���o�b�N���Ăяo��
		colliderB->OnCollision();
	}
};
