#include "IScene.h"

// タイトルシーンで初期化
int IScene::sceneNo = GAMESCENE;

bool IScene::GameRoop = false;
int IScene::AllStageTime[5][5] =
{ {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0} };
int IScene::menucount = 1;
bool IScene::menuposition = false;
// 仮想デストラクタの定義
// 純粋仮想関数化していないので、ここで定義できる
IScene::~IScene() {}

// シーン番号のゲッター
int IScene::GetSceneNo() { return sceneNo; }
void IScene::SetSceneNo(int sceneno) {
	sceneNo = sceneno;
}