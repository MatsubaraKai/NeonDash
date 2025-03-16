#include "GameScene.h"
#include "ImGuiCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "mathFunction.h"
#include "PSOPostEffect.h"
#include "Loader.h"
#include "Audio.h"
#include <iostream>
#include <cmath>
#include <DirectXMath.h>

// 初期化関数
void GameScene::Init()
{
	// カメラの初期化
	camera = new Camera;
	camera->Initialize();
	input = Input::GetInstance();

	// テクスチャのロード
	LoadTextures();

	// モデルのロード
	LoadModels();

	// オーディオのロード
	LoadAudio();

	// 必要なデータの初期化
	InitializeData();

	// パーティクルシステムの初期化
	InitializeParticles();

	// メニューとフェード処理の初期化
	menu = new Menu();
	menu->Init(textureHandles[MENUMEDI]);
	fade = new Fade();
	fade->Init(textureHandles[FADE]);

	postProcess_ = new PostProcess();
	postProcess_->SetCamera(camera);
	postProcess_->Init();
}

// シーン更新関数
void GameScene::Update() {
	// フェード処理の更新
	fade->UpdateFade();
	if (previousIsPreview && !isPreview) {
		timer.start();
	}
	previousIsPreview = isPreview;
	// プレイヤーの座標を取得
	Vector3 playerPos = camera->transform_.translate;

	// PositionOBJをプレイヤーの位置に合わせる
	PositionOBJ->worldTransform_.translation_ = playerPos;
	PositionOBJ->worldTransform_.translation_.y -= 2.99f;

	// ポータル判定
	UpdatePortalCollision(playerPos);

	// エフェクト管理
	UpdateEffects();
	// フェードアウト完了時のシーン遷移
	if (fade->IsFadeOutComplete()) {
		HandleSceneTransition();
	}
	MoveTextObjects(sceneTime);
	// TenQOBJの回転更新
	if (nowStage == 0) {
		UpdateTitleScene(playerPos, sceneTime);
		TenQOBJ->worldTransform_.rotation_.y += 0.0005f;
	}
	else {
		TenQOBJ->worldTransform_.rotation_.x += 0.001f;
		StarSetting(playerPos);
		StarCountNum();
	}
	// シーンタイムに応じた移動
	MoveConeObjects(sceneTime);
	// ステージタイムのモデル設定
	UpdateStageTimes();

	// 各オブジェクトをカメラに向ける
	AlignObjectsToCamera();

	// ゲームパッド入力処理
	HandleGamePadInput();

	// プレイヤーと床の衝突処理
	UpdatePlayerFloorCollision(playerPos);

	// 床の動きとプレイヤーの位置を同期
	UpdateFloorInteraction();

	// オブジェクトの更新処理
	UpdateObjects();

	// カメラ更新
	UpdateCamera();

	ClearMode();
	// デバッグ情報の表示
#ifdef _DEBUG
	DisplayDebugInfo(playerPos);
	ImguiDebug();
#endif
}

// 描画関数
void GameScene::Draw()
{
	// 各種オブジェクトの描画
	DrawConeObjects();
	DrawTitleTextObjects();
	DrawTitleNumberObjects();
	DrawSpecialObjects();

	// 特殊効果の描画
	DrawParticles();
	if (isMenu) {
		menu->Draw();
	}
	if (nowStage != 0) {
		StarCountNumber->Draw(textureHandles[GRID], camera);
	}

	// フェード描画
	fade->Draw();
}

// ポストエフェクト描画関数
void GameScene::PostDraw()
{
	postProcess_->Draw();
}

// リソース解放関数
void GameScene::Release() {
	Audio::SoundStopWave(Audio::GetInstance()->GetIXAudio().Get(), audioHandle[BGM]);
	Audio::SoundUnload(audioHandle[BGM]);
}

// ゲーム終了判定関数
int GameScene::GameClose()
{
	return false;
}

///Refactoring///

///Init///
// テクスチャのロード
void GameScene::LoadTextures()
{
	textureHandles[WHITE] = TextureManager::StoreTexture("Resources/white.png");
	textureHandles[PARTICLERED] = TextureManager::StoreTexture("Resources/particlered.png");
	textureHandles[PARTICLESTAR] = TextureManager::StoreTexture("Resources/particlestar.png");
	textureHandles[PARTICLEBLUE] = TextureManager::StoreTexture("Resources/particleblue.png");
	textureHandles[BLUE] = TextureManager::StoreTexture("Resources/blue.png");
	textureHandles[FADE] = TextureManager::StoreTexture("Resources/black.png");
	textureHandles[FADE2] = TextureManager::StoreTexture("Resources/black2.png");
	textureHandles[FADE3] = TextureManager::StoreTexture("Resources/black3.png");
	textureHandles[MENUMEDI] = TextureManager::StoreTexture("Resources/game/menumedi.png");
	textureHandles[MENUHIGH] = TextureManager::StoreTexture("Resources/game/menuhigh.png");
	textureHandles[MENULOW] = TextureManager::StoreTexture("Resources/game/menulow.png");
	textureHandles[GRID] = TextureManager::StoreTexture("Resources/cian.png");
	textureHandles[CONE] = TextureManager::StoreTexture("Resources/game/cone.png");
	textureHandles[TITLETENQ] = TextureManager::StoreTexture("Resources/game/world.png");
	textureHandles[GAMETENQ] = TextureManager::StoreTexture("Resources/game/world2.png");
	textureHandles[STAR] = TextureManager::StoreTexture("Resources/game/star.png");
	textureHandles[POSITION] = TextureManager::StoreTexture("Resources/game/position.png");
}

// モデルのロード
void GameScene::LoadModels()
{
	ModelManager::GetInstance()->LoadModel("Resources/game", "world.obj");
	ModelManager::GetInstance()->LoadModel("Resources/game", "world2.obj");
	ModelManager::GetInstance()->LoadModel("Resources/game", "position.obj");
	ModelManager::GetInstance()->LoadModel("Resources/game/Text", "text7.obj");

	for (int i = 0; i <= 9; i++) {
		ModelManager::GetInstance()->LoadModel("Resources/game/Number", std::to_string(i) + ".obj");
	}
	ModelManager::GetInstance()->LoadModel("Resources/game/Number", "colon.obj");
}

// オーディオのロード
void GameScene::LoadAudio()
{
	audioHandle[BGM] = Audio::SoundLoadWave("Resources/game/Audio/BGM.wav");
	audioHandle[PORTAL] = Audio::SoundLoadWave("Resources/game/Audio/portal.wav");
	audioHandle[TIMECOUNT] = Audio::SoundLoadWave("Resources/game/Audio/timecount.wav");
	audioHandle[TIMECOUNT2] = Audio::SoundLoadWave("Resources/game/Audio/timecount2.wav");
	audioHandle[FIREWORK] = Audio::SoundLoadWave("Resources/game/Audio/firework.wav");
	audioHandle[GETSTAR] = Audio::SoundLoadWave("Resources/game/Audio/GetSE.wav");

}

// 初期化データのセットアップ
void GameScene::InitializeData()
{
	if (!GameRoop) {
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "TitleScene", ConeObject_, camera);
		Loader::LoadJsonFileText("Resources", "TitleText", TextObject_);
		Loader::LoadJsonFileNumber("Resources", "TitleNumber", NumberObject_);
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), audioHandle[BGM], true, 0.05f);
		GameRoop = true;
	}
	previousPos.resize(ConeObject_.size());
	for (size_t i = 0; i < ConeObject_.size() - 1; i++) {
		previousPos[i] = ConeObject_[i]->worldTransform_.translation_;
	}

	TenQOBJ = new Object3d();
	TenQOBJ->Init();
	PositionOBJ = new Object3d();
	PositionOBJ->Init();
	StarCountNumber = new Object3d();
	StarCountNumber->Init();
	StarCountNumber->worldTransform_.scale_ = { 2.0f,2.0f,2.0f };
	PositionOBJ->SetModel("position.obj");
	TitleTenQTransform.Initialize();
	TitleTenQTransform.scale_ = { -300.0f, 300.0f, 300.0f };
	TenQOBJ->SetWorldTransform(TitleTenQTransform);
	TenQOBJ->SetModel("world.obj");
	GameTenQTransform.Initialize();
	GameTenQTransform.translation_.y = 2200.0f;
	GameTenQTransform.translation_.z = 700.0f;
	GameTenQTransform.scale_.x = -10.0f;
	GameTenQTransform.scale_.y = 10.0f;
	GameTenQTransform.scale_.z = 10.0f;

	std::vector<ParticleWorldTransformData> transforms = {
	{&TitleSceneWorldTransformPa,PortalPosition[0]},
	{&DemoSceneWorldTransformPa, PortalPosition[1]},
	{&DemoSceneWorldTransformDemoPa, PortalPosition[2]},
	{&GameSceneWorldTransformPa, PortalPosition[3]},
	{&GameScene2WorldTransformPa, PortalPosition[4]},
	{&GameScene3WorldTransformPa, PortalPosition[5]}
	};

	for (auto& data : transforms) {
		data.transform->Initialize();
		data.transform->translation_ = data.translation;
	}

	camera->transform_.translate = { 0.0f, -10.0f, -15000.0f };
	camera->transform_.rotate = { -0.2f, 0.0f, 0.0f };
	isPreview = false;
	isFadeInStarted = false;
	portal = 0;
	isClear = false;
	isMenu = false;
	isStart = true;
}

// パーティクルシステムの初期化
void GameScene::InitializeParticles()
{
	randRange_ = { { -50.0f, 50.0f }, { 1.0f, 1.0f },{ -65.0f, 38.0f } };

	emitter_.count = 8;
	emitter_.frequency = 0.2f;
	emitter_.transform.scale = { 0.2f, 0.2f, 0.2f };
	emitter_.transform.translate = { 0.0f, -1.0f, 0.0f };

	demoRandPro = { {1.0f, 4.0f}, {1.0f, 4.0f}, {1.0f, 4.0f} };

	ParticleEmitter_.count = 6;
	ParticleEmitter_.frequency = 0.02f;
	ParticleEmitter_.frequencyTime = 0.0f;
	ParticleEmitter_.transform.scale = { 0.5f, 0.5f, 0.5f };

	fireworkRange_ = { {-0.2f, 0.2f}, {-0.2f, 0.2f}, {-0.2f, 0.2f} };

	fireworkEmitter_.count = 8;
	fireworkEmitter_.frequency = 0.02f;
	fireworkEmitter_.frequencyTime = 0.0f;
	fireworkEmitter_.transform.translate = { -30.0f,0.0f,40.0f };
	fireworkEmitter_.transform.scale = { 0.5f, 0.5f, 0.5f };
	fireworkEmitter_.initialPosition.translate = { -30.0f,0.0f,40.0f };
	fireworkEmitter_.initialPosition.scale = { 0.5f, 0.5f, 0.5f };


	fireworkRange_2 = { {-0.2f, 0.2f}, {-0.2f, 0.2f}, {-0.2f, 0.2f} };

	fireworkEmitter_2.count = 8;
	fireworkEmitter_2.frequency = 0.02f;
	fireworkEmitter_2.frequencyTime = 0.0f;
	fireworkEmitter_2.transform.translate = { 30.0f,0.0f,40.0f };
	fireworkEmitter_2.transform.scale = { 0.5f, 0.5f, 0.5f };
	fireworkEmitter_2.initialPosition.translate = { 30.0f,0.0f,40.0f };
	fireworkEmitter_2.initialPosition.scale = { 0.5f, 0.5f, 0.5f };

	particles[FLOOR] = new Engine::Particle();
	particles[CLEAR] = new Engine::Particle();
	particles[TITLESCENE] = new Engine::Particle();
	particles[DEMOSCENE] = new Engine::Particle();
	particles[FAKEPARTICLE] = new Engine::Particle();
	particles[STAGE1] = new Engine::Particle();
	particles[STAGE2] = new Engine::Particle();
	particles[STAGE3] = new Engine::Particle();
	particles[FIREWORK1] = new Engine::Particle();
	particles[FIREWORK2] = new Engine::Particle();

	particles[FIREWORK1]->Initialize(fireworkEmitter_);
	particles[FIREWORK2]->Initialize(fireworkEmitter_2);

	particles[FLOOR]->Initialize(emitter_);
	particles[TITLESCENE]->Initialize(ParticleEmitter_);
	particles[DEMOSCENE]->Initialize(ParticleEmitter_);
	particles[FAKEPARTICLE]->Initialize(ParticleEmitter_);
	particles[STAGE1]->Initialize(ParticleEmitter_);
	particles[STAGE2]->Initialize(ParticleEmitter_);
	particles[STAGE3]->Initialize(ParticleEmitter_);
	particles[CLEAR]->Initialize(ParticleEmitter_);
}

///Update///
// ポータル判定
void GameScene::UpdatePortalCollision(const Vector3& playerPos) {
	if (nowStage != 0) {
		isTitle = collider->CheckCollision(playerPos, TitleSceneWorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
		if (StarCount[nowStage - 1] == 0) {
			isStageClear = collider->CheckCollision(playerPos, ClearPortalPosition[nowStage - 1], 2.5f, 4.0f, 2.5f, 2.0f);
		}
	}
	if (nowStage == 0) {
	    isDemo = collider->CheckCollision(playerPos, DemoSceneWorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
		isGame = collider->CheckCollision(playerPos, GameSceneWorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
		isGame2 = collider->CheckCollision(playerPos, GameScene2WorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
		isGame3 = collider->CheckCollision(playerPos, GameScene3WorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
	}
	// ポータルが一度だけ発動する処理
	if (isTitle || isDemo || isGame || isGame2 || isGame3 || isStageClear) {
		if (portal == 0) {
			Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), audioHandle[PORTAL], false, 0.1f);
		}
		portal++;
		isClear = true;
	}
	else {
		isClear = false;
	}
}

// エフェクト管理
void GameScene::UpdateEffects() {
	effect = (sceneTime == 180);
	effect2 = (sceneTime == 360);

	if (effect) IPostEffectState::SetEffectNo(kOutlinePurple);
	if (effect2) IPostEffectState::SetEffectNo(kOutlineBlue);

	if (nowStage != 0) {
		if (sceneTime == 180 || sceneTime == 360)
			Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), audioHandle[TIMECOUNT2], false, 1.0f);

		if (sceneTime == 60 || sceneTime == 120 || sceneTime == 240 || sceneTime == 300) {
			Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), audioHandle[TIMECOUNT], false, 1.0f);
		}
	}

	if (sceneTime >= 360) sceneTime = 0;
	if (sceneTime1 >= 360) sceneTime1 = 0;

	if (!isMenu) {
		sceneTime++;
		sceneTime1++;
	}
}

// シーン遷移処理
void GameScene::HandleSceneTransition() {
	if (isTitle || isStageClear) {
		Remake();
		isTitle = false;
		isStageClear = false;
		TenQOBJ->SetWorldTransform(TitleTenQTransform);
		TenQOBJ->SetModel("world.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "TitleScene", ConeObject_, camera);
		Loader::LoadJsonFileText("Resources", "TitleText", TextObject_);
		Loader::LoadJsonFileNumber("Resources", "TitleNumber", NumberObject_);
		Resize();
		nowStage = 0;
		portal = 0;
		fade->SetTexture(textureHandles[FADE]);
		fade->StartFadeOut();
		fade->SetAlpha(0.0f);
		isPreview = true;
	}
	else if (isDemo) {
		Remake();
		isDemo = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "DemoScene", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "DemoScene", StarObject_);
		Loader::LoadJsonFileText("Resources", "DemoText", TextObject_);
		Resize();
		FirstDemoFlag = true;
		nowStage = 1;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	else if (isGame) {
		Remake();
		isGame = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene1", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene1", StarObject_);
		Loader::LoadJsonFileText("Resources", "Stage1Text", TextObject_);
		Resize();
		nowStage = 2;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	else if (isGame2) {
		Remake();
		isGame2 = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene2", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene2", StarObject_);
		Loader::LoadJsonFileText("Resources", "Stage2Text", TextObject_);
		Resize();
		nowStage = 3;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	else if (isGame3) {
		Remake();
		isGame3 = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene3", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene3", StarObject_);
		Resize();
		nowStage = 4;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
}

// ステージタイムのモデル設定
void GameScene::UpdateStageTimes() {
	if (nowStage == 0) {
		for (int i = 0; i < 4; ++i) {
			NumberObject_[indices[i]]->SetModel((std::to_string(AllStageTime[0][i]) + ".obj").c_str());
			NumberObject_[indices[i] + 5]->SetModel((std::to_string(AllStageTime[1][i]) + ".obj").c_str());
			NumberObject_[indices[i] + 10]->SetModel((std::to_string(AllStageTime[2][i]) + ".obj").c_str());
			NumberObject_[indices[i] + 15]->SetModel((std::to_string(AllStageTime[3][i]) + ".obj").c_str());
		}
	}
}

// 各オブジェクトをカメラに向ける
void GameScene::AlignObjectsToCamera() {
		for (auto& obj : TextObject_) {
			obj->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, obj->worldTransform_.translation_) + 3.14f;
		}
		for (auto& obj : NumberObject_) {
			obj->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, obj->worldTransform_.translation_) + 3.14f;
		}
}

// ゲームパッド入力処理
void GameScene::HandleGamePadInput() {
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(joyState)) {
		HandleStartButton(joyState);
		if (isMenu) HandleMenuNavigation(joyState);
		else UpdateCameraFOV(joyState);
	}
}

// STARTボタンの入力を処理
void GameScene::HandleStartButton(const XINPUT_STATE& joyState) {
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
		if (!startButtonPressed) {
			isMenu = !isMenu;
			startButtonPressed = true;
		}
	}
	else {
		startButtonPressed = false;
	}
}

// メニュー操作の処理
void GameScene::HandleMenuNavigation(const XINPUT_STATE& joyState) {
	static WORD previousButtons = 0;
	WORD currentButtons = joyState.Gamepad.wButtons;

	if ((currentButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) && !(previousButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		if (menucount > 0) {
			menucount--;
			menu->SE();
		}
	}
	if ((currentButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && !(previousButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		if (menucount < 2) {
			menucount++;
			menu->SE();
		}
	}
	if ((currentButtons & XINPUT_GAMEPAD_Y) && !(previousButtons & XINPUT_GAMEPAD_Y)) {
		menuposition = !menuposition;
		menu->SE();
	}
	previousButtons = currentButtons;

	menu->ChangeTex((menucount == 0) ? textureHandles[MENULOW] :
		(menucount == 1) ? textureHandles[MENUMEDI] : textureHandles[MENUHIGH]);
}
// 床とのインタラクション処理
void GameScene::UpdateFloorInteraction()
{
	// プレイヤーの現在位置
	Vector3 playerPos = camera->transform_.translate;
	

	// 各床オブジェクトに対してチェック
	for (size_t i = 0; i < ConeObject_.size(); ++i) {
		if (ConeObject_[i]->isVisible) {

			const Vector3& floorPos = ConeObject_[i]->worldTransform_.translation_;
			const Vector3& floorSize = ConeObject_[i]->worldTransform_.scale_;

			// プレイヤーが床の上にいるかを判定
			if (playerPos.x >= floorPos.x - floorSize.x &&
				playerPos.x <= floorPos.x + floorSize.x &&
				playerPos.z >= floorPos.z - floorSize.z &&
				playerPos.z <= floorPos.z + floorSize.z &&
				playerPos.y >= floorPos.y + floorSize.y - 1.0f &&
				playerPos.y <= floorPos.y + floorSize.y + 3.0f)
			{
				// 床の移動量を計算
				Vector3 floorMovement = floorPos - previousPos[i];

				// プレイヤーに床の移動分を反映
				camera->transform_.translate.x += floorMovement.x;
				camera->transform_.translate.z += floorMovement.z;

				// プレイヤーの高さを床に合わせる
				camera->transform_.translate.y = floorPos.y + floorSize.y + 3.0f;

				// プレイヤーが床の上にいるフラグを設定
				isOnFloor = true;
				isStart = false;
			}

			// 床の現在位置を次フレーム用に保存
			previousPos[i] = floorPos;
		}
	}
		// 床の上にいない場合
		if (!isOnFloor) {
			isOnFloor = false;
		}
}

// カメラの更新
void GameScene::UpdateCamera() {
	// プレイヤーが床の上にいるかどうかでジャンプ処理を実行
	if (isClear == false && isMenu == false && isPreview == false) {
		camera->Jump(isOnFloor);
		camera->Move(menucount);
	}

	// フェードイン中であればカメラの移動を制御
	if (!isFadeInStarted && isClear == true) {
		if (collider->CheckCollision(camera->transform_.translate, ClearPortalPosition[nowStage - 1], 2.5f, 4.0f, 2.5f, 2.0f) && StarCount[nowStage - 1] == 0) {
			fade->SetTexture(textureHandles[FADE3]);
		}
		else {
			fade->SetTexture(textureHandles[FADE]);

		}
		fade->StartFadeIn();    // FadeInを開始
		isFadeInStarted = true; // フラグを立てて一度だけ実行されるようにする
	}
	if (isPreview == true) {
			camera->StagePreview(stageCenter[nowStage], stageRadius[nowStage], rotationSpeed[nowStage], angleX[nowStage], isPreview);
			isFadeInStarted = isPreview;
			if (camera->isEasing == true) {
				fade->SetAlpha(0.0f);
			}
	}
	else
	{
		camera->isEasing = false;
	}
	// カメラの更新処理を呼び出す
	camera->Update();
}

// FOV（視野角）の更新
void GameScene::UpdateCameraFOV(const XINPUT_STATE& joyState) {
	// 左スティックの入力値を取得
	Vector3 moveLeftStick = {
		(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
		0.0f,
		(float)joyState.Gamepad.sThumbLY / SHRT_MAX
	};

	// 入力の強度を計算
	float inputMagnitude = Length(moveLeftStick);

	// デッドゾーンを調整（スティックがある程度倒された場合のみ反応）
	const float deadZone = 0.2f; // スティック感度調整用の閾値
	if (inputMagnitude > deadZone) {
		moveLeftStick.x *= 0.004f;
		moveLeftStick.z *= 0.004f;
	}
	else {
		// デッドゾーン内なら FOV をデフォルト値に戻す
		moveLeftStick = { 0.0f, 0.0f, 0.0f };
	}

	// 前進入力が十分大きい場合のみ FOV を変更
	const float forwardThreshold = 0.00002f; // 前進入力の閾値
	float desiredFOV = (moveLeftStick.z > forwardThreshold) ? 1.0f : 0.8f;

	// FOV をスムーズに遷移
	if (isClear == false && isMenu == false && isPreview == false && isStart == false) {
		camera->SetFOV(Lerp(camera->fovY_, desiredFOV, 0.1f));
	}
}

// プレイヤーと床の衝突処理
void GameScene::UpdatePlayerFloorCollision(const Vector3& playerPos) {
	isOnFloor = false;
	for (size_t i = 0; i < ConeObject_.size(); ++i) {
		if (ConeObject_[i]->isVisible) {
		Vector3 floorPos = ConeObject_[i]->worldTransform_.translation_;
			Vector3 floorSize = ConeObject_[i]->worldTransform_.scale_;

			if (playerPos.x >= floorPos.x - floorSize.x && playerPos.x <= floorPos.x + floorSize.x &&
				playerPos.z >= floorPos.z - floorSize.z && playerPos.z <= floorPos.z + floorSize.z &&
				playerPos.y >= floorPos.y + floorSize.y - 1.0f && playerPos.y <= floorPos.y + floorSize.y + 3.0f) {
					float floorHeightChange = floorPos.y + floorSize.y - playerPos.y;
					camera->transform_.translate.y += floorHeightChange + 3.0f;
					isOnFloor = true;
					break;
			}
			else {
				isOnFloor = false;
			}
		}
	}
}

// オブジェクトの更新処理
void GameScene::UpdateObjects() {
	for (auto& obj : ConeObject_) {
		if ((obj)->isVisible) {
			obj->Update();
		}
	}
	for (auto& obj : TextObject_) obj->Update();
	for (auto& obj : NumberObject_) obj->Update();
	for (auto& obj : StarObject_) {
		if ((obj)->isVisible) {
			obj->Update();
			obj->worldTransform_.rotation_.y += 0.02f;
		}
	} 
	TenQOBJ->Update();
	PositionOBJ->Update();
}


///Draw///
// コーンオブジェクトの描画
void GameScene::DrawConeObjects()
{
	for (auto& cone : ConeObject_) {
		if (cone->isVisible) {
			cone->Draw(textureHandles[CONE], camera);
		}
	}
	for (auto& star : StarObject_) {
		if (star->isVisible) {
			star->Draw(textureHandles[STAR], camera);
		}
	}
}

// タイトルテキストの描画
void GameScene::DrawTitleTextObjects()
{
	for (size_t i = 0; i < TextObject_.size(); ++i) {
		auto& textObj = TextObject_[i];
		if (i == 0 || i == 1) {
			textObj->Draw(textureHandles[BLUE], camera); // 特定のオブジェクトは異なるテクスチャで描画
		}
		else {
			textObj->Draw(textureHandles[GRID], camera);
		}
	}
}

// タイトルナンバーの描画
void GameScene::DrawTitleNumberObjects()
{
	for (auto& numberObj : NumberObject_) {
		numberObj->Draw(textureHandles[GRID], camera);
	}
}

// 特殊オブジェクトの描画
void GameScene::DrawSpecialObjects()
{
	if (isTitle == false) {
		TenQOBJ->Draw(textureHandles[TITLETENQ], camera);
	}
	else {
		TenQOBJ->Draw(textureHandles[GAMETENQ], camera);
	}
	if (menuposition) {
		PositionOBJ->Draw(textureHandles[POSITION], camera);
	}
}

// パーティクルの描画
void GameScene::DrawParticles()
{
	if (nowStage != 0) {
		particles[CLEAR]->Draw(
			ParticleEmitter_,
			ClearPortalPosition[nowStage - 1],
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);
	}
	if (nowStage == 0) {
		particles[FLOOR]->Draw(emitter_, emitter_.transform.translate, textureHandles[PARTICLEBLUE], camera, randRange_, false, 0.5f, 0.8f);
		particles[DEMOSCENE]->Draw(
			ParticleEmitter_,
			{ DemoSceneWorldTransformPa.translation_.x, DemoSceneWorldTransformPa.translation_.y, DemoSceneWorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		particles[STAGE1]->Draw(
			ParticleEmitter_,
			{ GameSceneWorldTransformPa.translation_.x, GameSceneWorldTransformPa.translation_.y, GameSceneWorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		particles[STAGE2]->Draw(
			ParticleEmitter_,
			{ GameScene2WorldTransformPa.translation_.x, GameScene2WorldTransformPa.translation_.y, GameScene2WorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		particles[STAGE3]->Draw(
			ParticleEmitter_,
			{ GameScene3WorldTransformPa.translation_.x, GameScene3WorldTransformPa.translation_.y, GameScene3WorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		particles[FIREWORK1]->Draw(fireworkEmitter_, fireworkEmitter_.transform.translate, textureHandles[PARTICLESTAR], camera, fireworkRange_, false, 0.1f, 0.6f);
		particles[FIREWORK2]->Draw(fireworkEmitter_2, fireworkEmitter_2.transform.translate, textureHandles[PARTICLESTAR], camera, fireworkRange_2, false, 0.1f, 0.6f);
		particles[FIREWORK1]->CreateFireworkEffect(fireworkEmitter_, fireworkRange_, 1.0f, 2.0f, 1.5f, audioHandle[FIREWORK], camera->GetTranslate());
		particles[FIREWORK2]->CreateFireworkEffect(fireworkEmitter_2, fireworkRange_2, 1.0f, 2.0f, 1.5f, audioHandle[FIREWORK], camera->GetTranslate());
	}
	if (nowStage == 1) {
		particles[FAKEPARTICLE]->Draw(
			ParticleEmitter_,
			{ DemoSceneWorldTransformDemoPa.translation_.x, DemoSceneWorldTransformDemoPa.translation_.y, DemoSceneWorldTransformDemoPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);
	}
	if (nowStage != 0) {
		particles[TITLESCENE]->Draw(
			ParticleEmitter_,
			{ TitleSceneWorldTransformPa.translation_.x, TitleSceneWorldTransformPa.translation_.y, TitleSceneWorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);
	}
}

void GameScene::InitStar() {
	for (auto& star : StarObject_) {
		star->isVisible = true;
	}
	static const std::unordered_map<int, int> stageToStarCount = {
		{1, 2},
		{2, 3},
		{3, 4},
		{4, 5}
	};
	if (stageToStarCount.contains(nowStage)) {
		StarCount[nowStage - 1] = stageToStarCount.at(nowStage);
	}
}

void GameScene::Remake() {
	for (auto object : ConeObject_) {
		delete object;  // メモリの解放
	}
	for (auto object : StarObject_) {
		delete object;  // メモリの解放
	}
	for (auto object : TextObject_) {
		delete object;  // メモリの解放
	}
	for (auto object : NumberObject_) {
		delete object;  // メモリの解放
	}
	ConeObject_.clear(); // コンテナを空にする
	StarObject_.clear(); // コンテナを空にする
	TextObject_.clear(); // コンテナを空にする
	NumberObject_.clear(); // コンテナを空にする
}


// 指定範囲内にプレイヤーがいるか判定
bool GameScene::IsPlayerInRange(const Vector3& pos, float minX, float maxX, float minZ, float maxZ) {
	return pos.x >= minX && pos.x <= maxX && pos.z >= minZ && pos.z <= maxZ;
}

// Lerp適用
void GameScene::ApplyLerp(float& target, float goal, float speed) {
	target = Lerp(target, goal, speed);
}

// タイトルテキストの高さ調整
void GameScene::MoveTextObjects(int sceneTime) {
	if (sceneTime < 180) {
		if (nowStage == 0) {
			for (int i = 0; i < 6; i++) {
				ApplyLerp(TextObject_[i]->worldTransform_.translation_.y, Textlerpindices[i], 0.01f);
			}
		}
		if (nowStage == 1) {
			for (int i = 0; i < 6; i++) {
				ApplyLerp(TextObject_[indices1[i]]->worldTransform_.translation_.y, Textlerpindices1[i], 0.01f);
			}
		}
	}
	else if (sceneTime < 360) {
		if (nowStage == 0) {
			for (int i = 0; i < 6; i++) {
				ApplyLerp(TextObject_[i]->worldTransform_.translation_.y, textlerpindices[i], 0.01f);
			}
		}
		if (nowStage == 1) {
			for (int i = 0; i < 6; i++) {
				ApplyLerp(TextObject_[indices1[i]]->worldTransform_.translation_.y, textlerpindices1[i], 0.01f);
			}
		}
	}
}

// タイトルナンバーオブジェクトの高さ調整
void GameScene::MoveTitleNumberObjects(float targetHeight) {
	for (auto& obj : NumberObject_) {
		ApplyLerp(obj->worldTransform_.translation_.y, targetHeight, 0.01f);
	}
	
}

// コーンオブジェクトの移動
void GameScene::MoveConeObjects(int sceneTime) {
	bool firstPhase = (sceneTime < 180);
	bool secondPhase = (sceneTime >= 180 && sceneTime < 360);

	if (nowStage == 0) {

		MoveInCircle(ConeObject_[19]->worldTransform_.translation_, deltatime, 100.0f, center, angle, 0.01f, 0);
		MoveInCircle(ConeObject_[20]->worldTransform_.translation_, deltatime, 110.0f, center, angle, 0.01f, 1);
		MoveInCircle(ConeObject_[21]->worldTransform_.translation_, deltatime, 120.0f, center, angle, 0.01f, 2);

		if (firstPhase) {
			ApplyLerp(ConeObject_[17]->worldTransform_.translation_.y, 60.0f, 0.034f);
			ApplyLerp(ConeObject_[18]->worldTransform_.translation_.x, 55.0f, 0.034f);
			MoveTitleNumberObjects(8.5f);
		}
		else if (secondPhase) {
			ApplyLerp(ConeObject_[17]->worldTransform_.translation_.y, -4.0f, 0.034f);
			ApplyLerp(ConeObject_[18]->worldTransform_.translation_.x, -50.0f, 0.034f);
			MoveTitleNumberObjects(7.5f);
		}
	}
	if (nowStage == 1) {
		for (int i = 1; i <= 2; ++i) {
			float sign = (i == 1) ? 1.0f : -1.0f;
			ConeObject_[i]->worldTransform_.rotation_.x += 0.01f;
			ConeObject_[i]->worldTransform_.rotation_.y += sign * 0.01f;
			ConeObject_[i]->worldTransform_.rotation_.z += sign * 0.01f;
		}

	}
	if (nowStage == 2) {
		if (firstPhase) {
			ApplyLerp(ConeObject_[1]->worldTransform_.translation_.z, 0.0f, 0.02f);
			ApplyLerp(ConeObject_[3]->worldTransform_.translation_.y, -7.0f, 0.02f);
			ApplyLerp(StarObject_[1]->worldTransform_.translation_.y, 1.0f, 0.02f);

		}
		else if (secondPhase) {
			ApplyLerp(ConeObject_[1]->worldTransform_.translation_.z, 100.0f, 0.02f);
			ApplyLerp(ConeObject_[3]->worldTransform_.translation_.y, 55.0f, 0.02f);
			ApplyLerp(StarObject_[1]->worldTransform_.translation_.y, 63.0f, 0.02f);
		}
	}
	if (nowStage == 3) {
		if (firstPhase) {
			for (int i = 0; i < 14; i++) {
				ConeObject_[Stage2indices[i]]->isVisible = false;
			}
			for (int i = 0; i < 13; i++) {
				ConeObject_[Stage2indices2[i]]->isVisible = true;
			}
		}
		else if (secondPhase) {
			for (int i = 0; i < 14; i++) {
				ConeObject_[Stage2indices[i]]->isVisible = true;
			}
			for (int i = 0; i < 13; i++) {
				ConeObject_[Stage2indices2[i]]->isVisible = false;
			}
		}
	}
	if (nowStage == 4) {
		if (firstPhase){
			ApplyLerp(ConeObject_[1]->worldTransform_.translation_.y, -4.0f, 0.04f);
			for (int i = 0; i < 2; i++) {
				ApplyLerp(ConeObject_[Stage3indices[i]]->worldTransform_.translation_.x, conelerpindices[i], 0.02f);
			}
			ApplyLerp(ConeObject_[14]->worldTransform_.translation_.x, 90.0f, 0.03f);
			ApplyLerp(ConeObject_[14]->worldTransform_.translation_.y, 51.0f, 0.03f);

			ApplyLerp(ConeObject_[15]->worldTransform_.translation_.z, 182.0f, 0.03f);
			ApplyLerp(ConeObject_[15]->worldTransform_.translation_.y, 96.0f, 0.03f);

			ApplyLerp(ConeObject_[17]->worldTransform_.translation_.y, -15.0f, 0.03f);

		}
		else if (secondPhase) {
			ApplyLerp(ConeObject_[1]->worldTransform_.translation_.y, 50.0f, 0.04f);

			for (int i = 0; i < 2; i++) {
				ApplyLerp(ConeObject_[Stage3indices[i]]->worldTransform_.translation_.x, Conelerpindices[i], 0.02f);
			}
			ApplyLerp(ConeObject_[14]->worldTransform_.translation_.x, 110.0f, 0.03f);
			ApplyLerp(ConeObject_[14]->worldTransform_.translation_.y, 71.0f, 0.03f);

			ApplyLerp(ConeObject_[15]->worldTransform_.translation_.z, 157.0f, 0.03f);
			ApplyLerp(ConeObject_[15]->worldTransform_.translation_.y, 71.0f, 0.03f);

			ApplyLerp(ConeObject_[17]->worldTransform_.translation_.y, -4.0f, 0.03f);

		}
	}
}

// メイン処理
void GameScene::UpdateTitleScene(const Vector3& playerPos, int sceneTime) {
	// プレイヤー範囲によるテキスト移動
	if (FirstDemoFlag == false) {
		float targetHeight = IsPlayerInRange(playerPos, -20.0f, 20.0f, -20.0f, 20.0f) ? 1.3f : 0.0f;
		ApplyLerp(TextObject_[6]->worldTransform_.translation_.y, targetHeight, 0.1f);
	}
	// タイトルテキスト移動
	ApplyLerp(TextObject_[7]->worldTransform_.translation_.y, 2.0f, 0.1f);

}

void GameScene::StarSetting(const Vector3& playerPos) {
	for (size_t i = 0; i < StarObject_.size(); i++) {
		if (StarObject_[i]->isVisible) {
			// オブジェクトの座標とサイズを取得
			Vector3 starPos = StarObject_[i]->worldTransform_.translation_;
			Vector3 starSize = StarObject_[i]->worldTransform_.scale_;

			// プレイヤーがオブジェクトに当たっているか判定
			if (playerPos.x >= starPos.x - starSize.x - 2.0f &&
				playerPos.x <= starPos.x + starSize.x + 2.0f &&
				playerPos.z >= starPos.z - starSize.z - 2.0f &&
				playerPos.z <= starPos.z + starSize.z + 2.0f &&
				playerPos.y >= starPos.y + starSize.y - 5.0f &&
				playerPos.y <= starPos.y + starSize.y + 5.0f) {
				isGetStar = true;
				StarObject_[i]->isVisible = false;  // 該当オブジェクトの描画を停止
				break;  // 判定を終了
			}
			else {
				isGetStar = false;
			}
		}
		else {
			isGetStar = false;
		}
	}
	if (isGetStar) {
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), audioHandle[GETSTAR], false, 1.0f);
		StarCount[nowStage - 1]--;
	}
}

void GameScene::ClearMode() {
	if (collider->CheckCollision(camera->transform_.translate, ClearPortalPosition[nowStage - 1], 2.5f, 4.0f, 2.5f, 2.0f) && StarCount[nowStage - 1] == 0) {
		if (AllStageTime[nowStage - 1][4] == 0) {
			AllStageTime[nowStage - 1][0] = timer.elapsedTensOfMinutes();
			AllStageTime[nowStage - 1][1] = timer.elapsedMinutesOnly();
			AllStageTime[nowStage - 1][2] = timer.elapsedTensOfSeconds();
			AllStageTime[nowStage - 1][3] = timer.elapsedSecondsOnly();
			AllStageTime[nowStage - 1][4] = static_cast<int>(timer.elapsedSeconds());
		}
		if (static_cast<int>(timer.elapsedSeconds()) < AllStageTime[nowStage - 1][4]) {//前回の記録より早かったら記録
			AllStageTime[nowStage - 1][0] = timer.elapsedTensOfMinutes();
			AllStageTime[nowStage - 1][1] = timer.elapsedMinutesOnly();
			AllStageTime[nowStage - 1][2] = timer.elapsedTensOfSeconds();
			AllStageTime[nowStage - 1][3] = timer.elapsedSecondsOnly();
			AllStageTime[nowStage - 1][4] = static_cast<int>(timer.elapsedSeconds());
		}
		timer.stop();//タイマー止める
		fade->SetTexture(textureHandles[FADE3]);
	}
}

void GameScene::Resize() {
	previousPos.resize(ConeObject_.size());
	for (size_t i = 0; i < ConeObject_.size() - 1; i++) {
		previousPos[i] = ConeObject_[i]->worldTransform_.translation_;
	}
}

void GameScene::StarCountNum() {
	std::string modelFileName = std::to_string(StarCount[nowStage - 1]) + ".obj";
	StarCountNumber->SetModel(modelFileName.c_str());
	StarCountNumber->worldTransform_.translation_ = ClearNumberPosition[nowStage - 1];
	StarCountNumber->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, StarCountNumber->worldTransform_.translation_) + 3.14f;
	StarCountNumber->Update();
}

void GameScene::MoveInCircle(Vector3& FloorPos, float deltaTime, float radius,
	const Vector3 centerPos, float& angle, float speed, int mode) {
	// 角度を更新（常に通常回転）
	angle += speed * deltaTime;
	

	// 角度を 0～2π の範囲に制限
	if (angle > DirectX::XM_2PI) {
		angle -= DirectX::XM_2PI;
	}
	else if (angle < 0.0f) {
		angle += DirectX::XM_2PI;
	}

	// 円運動のパターンを mode で選択
	switch (mode) {
	case 0: // XZ平面（通常の横回転）
		FloorPos.x = centerPos.x + radius * std::cos(angle);
		FloorPos.z = centerPos.z + radius * std::sin(angle);
		FloorPos.y = centerPos.y;  // 高さは固定
		break;

	case 1: // YZ平面（縦回転）
		FloorPos.y = centerPos.y + radius * std::cos(angle);
		FloorPos.z = centerPos.z + radius * std::sin(angle);
		FloorPos.x = centerPos.x;  // X軸は固定
		break;

	case 2: // XY平面（Z軸の高さを固定した円運動）
		FloorPos.x = centerPos.x + radius * std::cos(angle);
		FloorPos.y = centerPos.y + radius * std::sin(angle);
		FloorPos.z = centerPos.z;  // Z軸の位置は固定
		break;
	}
}


// デバッグ情報の表示
void GameScene::DisplayDebugInfo(const Vector3& playerPos) {
#ifdef _DEBUG

	ImGui::Begin("Debug Info");
	ImGui::Text("Player Position: X=%f, Y=%f, Z=%f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::Text("On Floor: %d", isOnFloor);
	ImGui::End();
	ImGui::Begin("isOnFloor");
	ImGui::SliderInt("Select ConeModel Index", &selectedIndex1, 0, static_cast<int>(ConeObject_.size()) - 1);
	ImGui::SliderInt("Select StarModel Index", &selectedIndex2, 0, static_cast<int>(StarObject_.size()) - 1);
	ImGui::SliderInt("Select TextModel Index", &selectedIndex3, 0, static_cast<int>(TextObject_.size()) - 1);
	ImGui::Text("OnFloor : %d", isOnFloor);
	ImGui::Text("GetStar : %d", isGetStar);
	ImGui::Text("Player Pos : %f %f %f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::End();
	
	if (ConeObject_.size() > 0) {
		std::string label1 = "JSONConemodel" + std::to_string(selectedIndex1);
		ConeObject_[selectedIndex1]->ModelDebug(label1.c_str());
	}
	if (StarObject_.size() > 0) {
		std::string label2 = "JSONStarmodel" + std::to_string(selectedIndex2);
		StarObject_[selectedIndex2]->ModelDebug(label2.c_str());
	}
	if (TextObject_.size() > 0) {
		std::string label3 = "JSONTextmodel" + std::to_string(selectedIndex3);
		TextObject_[selectedIndex3]->ModelDebug(label3.c_str());
	}

	ImGui::Begin("stage");
	if (ImGui::Button("isTitle")) {
		Remake();
		isTitle = false;
		isStageClear = false;
		TenQOBJ->SetWorldTransform(TitleTenQTransform);
		TenQOBJ->SetModel("world.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "TitleScene", ConeObject_, camera);
		Loader::LoadJsonFileText("Resources", "TitleText", TextObject_);
		Loader::LoadJsonFileNumber("Resources", "TitleNumber", NumberObject_);
		Resize();
		nowStage = 0;
		portal = 0;
		fade->SetTexture(textureHandles[FADE]);
		fade->StartFadeOut();
		fade->SetAlpha(0.0f);
		isPreview = true;
	}
	if (ImGui::Button("isDemo")) {
		Remake();
		isDemo = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "DemoScene", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "DemoScene", StarObject_);
		Loader::LoadJsonFileText("Resources", "DemoText", TextObject_);
		Resize();
		FirstDemoFlag = true;
		nowStage = 1;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	if (ImGui::Button("isGame")) {
		Remake();
		isGame = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene1", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene1", StarObject_);
		Resize();
		nowStage = 2;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	if (ImGui::Button("isGame2")) {
		Remake();
		isGame2 = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene2", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene2", StarObject_);
		Resize();
		nowStage = 3;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	if (ImGui::Button("isGame3")) {
		Remake();
		isGame3 = false;
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene3", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene3", StarObject_);
		Resize();
		nowStage = 4;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	ImGui::End();

	for (size_t i = 0; i < ConeObject_.size() - 1; i++) {
		float previousFloorHeight = playerPos.y; // 初期化しておく
		// オブジェクトの座標とサイズを取得
		Vector3 floorPos = ConeObject_[i]->worldTransform_.translation_;
		Vector3 floorSize = ConeObject_[i]->worldTransform_.scale_;
		std::string label = "JSONmodel" + std::to_string(i);

		ImGui::Begin("OnFloorDebug");
		ImGui::Text(label.c_str());
		ImGui::Text("floor : %f %f %f", floorPos.x, floorPos.y, floorPos.z);
		ImGui::Text("size : %f %f %f", floorSize.x, floorSize.y, floorSize.z);
		ImGui::Text("isOnx : %f %f", playerPos.x, floorPos.x - floorSize.x);
		ImGui::Text("isOnx : %f %f", playerPos.x, floorPos.x + floorSize.x);
		ImGui::Text("isOnz : %f %f", playerPos.z, floorPos.z - floorSize.z);
		ImGui::Text("isOnz : %f %f", playerPos.z, floorPos.z + floorSize.z);
		ImGui::Text("isOny : %f %f", playerPos.y, abs(floorPos.y + floorSize.y + 3.0f));
		ImGui::Text("isOnyy : %f", abs(playerPos.y - (floorPos.y + floorSize.y + 3.0f)));
		ImGui::End();
		// プレイヤーがオブジェクトの上にいるか判定
		if (playerPos.x >= floorPos.x - floorSize.x &&
			playerPos.x <= floorPos.x + floorSize.x &&
			playerPos.z >= floorPos.z - floorSize.z &&
			playerPos.z <= floorPos.z + floorSize.z &&
			playerPos.y >= floorPos.y + floorSize.y - 1.0f &&
			playerPos.y <= floorPos.y + floorSize.y + 3.0f) {
			break;
		}
		else {
			isOnFloor = false;
		}
	}
	camera->CameraDebug();
#endif

}

void GameScene::ImguiDebug() {
	TenQOBJ->ModelDebug("TenQ");
	ImGui::Begin("bool");
	ImGui::Text("nowstage %d", nowStage);
	ImGui::Text("isTitle %d", isTitle);
	ImGui::Text("isDemo %d", isDemo);
	ImGui::Text("isGame %d", isGame);
	ImGui::Text("isGame2 %d", isGame2);
	ImGui::Text("isGame3 %d", isGame3);
	ImGui::Text("portal %d", portal);
	ImGui::Text("isclear %d", isClear);	
	ImGui::Text("isFadeInStarted %d", isFadeInStarted);
	ImGui::Text("isPreview %d", isPreview);
	ImGui::Text("previousIsPreview %d", previousIsPreview);
	if (nowStage != 0) {
		ImGui::Text("starcount %d", StarCount[nowStage - 1]);
		ImGui::Text("isvisible: %d", StarObject_[selectedIndex2]->isVisible);
		for (size_t i = 0; i < ConeObject_.size(); ++i) {
			ImGui::Text("Coneisvisible: %d", ConeObject_[i]->isVisible);
		}
	}
	ImGui::Text("isgetstar %d", isGetStar);
	ImGui::Text("time %d%d:%d%d", timer.elapsedTensOfMinutes(),timer.elapsedMinutesOnly(),timer.elapsedTensOfSeconds(),timer.elapsedSecondsOnly());
	ImGui::Text("size %d", previousPos.size());
	ImGui::End();
}

