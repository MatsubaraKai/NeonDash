#include "GameScene.h"

// コンストラクタ
GameScene::GameScene() : currentStage_(Stage_Title) {}

// ステージを切り替える
void GameScene::SetStage(StageID stageID) {
    currentStage_ = stageID;
    Init();
}

// ステージ初期化
void GameScene::Init() {
    switch (currentStage_) {
    case Stage_Title:
        std::cout << "Initializing Title Stage\n";
        break;
    case Stage_1:
        std::cout << "Initializing Stage 1\n";
        break;
    case Stage_2:
        std::cout << "Initializing Stage 2\n";
        break;
    case Stage_3:
        std::cout << "Initializing Stage 3\n";
        break;
    default:
        break;
    }
}

// 更新処理
void GameScene::Update() {
    // 共通処理
    AllInit();
    HandlePlayerInput();
    UpdatePhysics();
    UpdateCamera();

    // ステージ固有の処理
    UpdateStageSpecific();
}


void GameScene::AllInit() {

}

// 共通処理: プレイヤー入力
void GameScene::HandlePlayerInput() {
    std::cout << "Handling player input\n";
    // 例: キー入力、コントローラ入力処理
}

// 共通処理: 物理計算や当たり判定
void GameScene::UpdatePhysics() {
    std::cout << "Updating physics and collision\n";

    // 例: 当たり判定や物理計算
}

// 共通処理: カメラ更新
void GameScene::UpdateCamera() {
    std::cout << "Updating camera\n";
    // 例: カメラの位置や視野の更新
}

// ステージ固有の更新処理
void GameScene::UpdateStageSpecific() {
    switch (currentStage_) {
    case Stage_Title:
        std::cout << "Updating Title Stage Specific Logic\n";
        break;
    case Stage_1:
        std::cout << "Updating Stage 1 Specific Logic\n";
        break;
    case Stage_2:
        std::cout << "Updating Stage 2 Specific Logic\n";
        break;
    case Stage_3:
        std::cout << "Updating Stage 3 Specific Logic\n";
        break;
    default:
        break;
    }
}

// 描画処理
void GameScene::Draw() {
    // 共通描画処理
    std::cout << "Drawing common objects\n";

    // ステージ固有の描画
    switch (currentStage_) {
    case Stage_Title:
        std::cout << "Drawing Title-specific elements\n";
        break;
    case Stage_1:
        std::cout << "Drawing Stage 1-specific elements\n";
        break;
    case Stage_2:
        std::cout << "Drawing Stage 2-specific elements\n";
        break;
    case Stage_3:
        std::cout << "Drawing Stage 3-specific elements\n";
        break;
    default:
        break;
    }
}


// ポストエフェクト描画関数
void GameScene::PostDraw()
{
}

// リソース解放関数
void GameScene::Release() {
}

// ゲーム終了判定関数
int GameScene::GameClose()
{
    return false;
}

GameScene* GameScene::GetInstance() {
    static GameScene instance;
    return &instance;
}