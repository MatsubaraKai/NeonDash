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
	}
	if (nowStage == 1) {

	}
	if (nowStage == 2) {

	}
	if (nowStage == 3) {

	}
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

	// 文字床アニメーション
	//UpdateLerpAnimations(playerPos);

	// オブジェクトの更新処理
	UpdateObjects();

	// カメラ更新
	UpdateCamera();

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
	Audio::SoundStopWave(Audio::GetInstance()->GetIXAudio().Get(), AudioBGMhandle_);
	Audio::SoundUnload(AudioBGMhandle_);
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
	AudioBGMhandle_ = Audio::SoundLoadWave("Resources/game/Audio/BGM.wav");
	AudioPortalhandle_ = Audio::SoundLoadWave("Resources/game/Audio/portal.wav");
	AudioTimeCounthandle_ = Audio::SoundLoadWave("Resources/game/Audio/timecount.wav");
	AudioTimeCount2handle_ = Audio::SoundLoadWave("Resources/game/Audio/timecount2.wav");
	explosionSound = Audio::SoundLoadWave("Resources/game/Audio/firework.wav");
	AudioStarGetSEhandle_ = Audio::SoundLoadWave("Resources/game/Audio/GetSE.wav");

}

// 初期化データのセットアップ
void GameScene::InitializeData()
{
	if (!GameRoop) {
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "TitleScene", ConeObject_, camera);
		Loader::LoadJsonFileText("Resources", "TitleText", TextObject_);
		Loader::LoadJsonFileNumber("Resources", "TitleNumber", NumberObject_);
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioBGMhandle_, true, 0.05f);
		GameRoop = true;
	}

	for (size_t i = 0; i < ConeObject_.size() - 1; i++) {
		previousPos[i] = ConeObject_[i]->worldTransform_.translation_;
	}

	TenQOBJ = new Object3d();
	TenQOBJ->Init();
	PositionOBJ = new Object3d();
	PositionOBJ->Init();
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
	{&TitleSceneWorldTransformPa, {-20.0f, 1.5f, -17.5f}},
	{&DemoSceneWorldTransformPa, {-25.0f, 1.5f, 12.5f}},
	{&DemoSceneWorldTransformDemoPa, {-2.5f, 1.5f, -32.35f}},
	{&GameSceneWorldTransformPa, {25.0f, 1.5f, 12.5f}},
	{&GameScene2WorldTransformPa, {25.0f, 1.5f, 0.0f}},
	{&GameScene3WorldTransformPa, {25.0f, 1.5f, -12.5f}}
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
	TitleFireworkPa = new Engine::Particle();
	TitleFireworkPa->Initialize(fireworkEmitter_);
	TitleFireworkPa2 = new Engine::Particle();
	TitleFireworkPa2->Initialize(fireworkEmitter_2);

	FloorParticle_ = new Engine::Particle();
	FloorParticle_->Initialize(emitter_);
	TitleSceneParticle = new Engine::Particle();
	TitleSceneParticle->Initialize(ParticleEmitter_);
	DemoSceneParticle = new Engine::Particle();
	DemoSceneParticle->Initialize(ParticleEmitter_);
	DemoSceneDemoParticle = new Engine::Particle();
	DemoSceneDemoParticle->Initialize(ParticleEmitter_);
	Stage1Particle = new Engine::Particle();
	Stage1Particle->Initialize(ParticleEmitter_);
	Stage2Particle = new Engine::Particle();
	Stage2Particle->Initialize(ParticleEmitter_);
	Stage3Particle = new Engine::Particle();
	Stage3Particle->Initialize(ParticleEmitter_);

}

///Update///
// ポータル判定
void GameScene::UpdatePortalCollision(const Vector3& playerPos) {
	if(nowStage != 0)
	    isTitle = collider->CheckCollision(playerPos, TitleSceneWorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
	    
	if (nowStage == 0) {
	    isDemo = collider->CheckCollision(playerPos, DemoSceneWorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
		isGame = collider->CheckCollision(playerPos, GameSceneWorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
		isGame2 = collider->CheckCollision(playerPos, GameScene2WorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
		isGame3 = collider->CheckCollision(playerPos, GameScene3WorldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
	}

	// ポータルが一度だけ発動する処理
	if (isTitle || isDemo || isGame || isGame2 || isGame3) {
		if (portal == 0) {
			Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioPortalhandle_, false, 0.1f);
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
			Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioTimeCount2handle_, false, 1.0f);

		if (sceneTime == 60 || sceneTime == 120 || sceneTime == 240 || sceneTime == 300) {
			Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioTimeCounthandle_, false, 1.0f);
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
	if (isTitle) {
		Remake();
		isTitle = false;
		TenQOBJ->SetWorldTransform(TitleTenQTransform);
		TenQOBJ->SetModel("world.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "TitleScene", ConeObject_, camera);
		Loader::LoadJsonFileText("Resources", "TitleText", TextObject_);
		Loader::LoadJsonFileNumber("Resources", "TitleNumber", NumberObject_);
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
			NumberObject_[indices[i]]->SetModel((std::to_string(DemoTime[i]) + ".obj").c_str());
			NumberObject_[indices[i] + 5]->SetModel((std::to_string(SCENE1Time[i]) + ".obj").c_str());
			NumberObject_[indices[i] + 10]->SetModel((std::to_string(SCENE2Time[i]) + ".obj").c_str());
			NumberObject_[indices[i] + 15]->SetModel((std::to_string(SCENE3Time[i]) + ".obj").c_str());
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
		}

		// 床の現在位置を次フレーム用に保存
		previousPos[i] = floorPos;
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
		fade->SetTexture(textureHandles[FADE]);
		fade->StartFadeIn();    // フェードインを開始
		isFadeInStarted = true; // フラグを立て、一度だけ実行
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
	if (isClear == false && isMenu == false && isPreview == false) {
		camera->SetFOV(Lerp(camera->fovY_, desiredFOV, 0.1f));
	}
}

// プレイヤーと床の衝突処理
void GameScene::UpdatePlayerFloorCollision(const Vector3& playerPos) {
	isOnFloor = false;
	for (size_t i = 0; i < ConeObject_.size(); ++i) {
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

// オブジェクトの更新処理
void GameScene::UpdateObjects() {
	for (auto& obj : ConeObject_) obj->Update();
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

// デバッグ情報の表示
void GameScene::DisplayDebugInfo(const Vector3& playerPos) {
#ifdef _DEBUG

	ImGui::Begin("Debug Info");
	ImGui::Text("Player Position: X=%f, Y=%f, Z=%f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::Text("On Floor: %d", isOnFloor);
	ImGui::End();
	ImGui::Begin("isOnFloor");
	ImGui::SliderInt("Select Model Index1", &selectedIndex1, 0, static_cast<int>(ConeObject_.size()) - 1);
	ImGui::SliderInt("Select Model Index2", &selectedIndex2, 0, static_cast<int>(StarObject_.size()) - 1);
	ImGui::Text("OnFloor : %d", isOnFloor);
	ImGui::Text("GetStar : %d", isGetStar);
	ImGui::Text("Player Pos : %f %f %f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::End();

	std::string label1 = "JSONConemodel" + std::to_string(selectedIndex1);
	ConeObject_[selectedIndex1]->ModelDebug(label1.c_str());
	if (nowStage != 0) {
		std::string label2 = "JSONStarmodel" + std::to_string(selectedIndex2);
		StarObject_[selectedIndex2]->ModelDebug(label2.c_str());
	}
	ImGui::Begin("stage");
	if (ImGui::Button("isTitle")) {
		Remake();
		TenQOBJ->SetWorldTransform(TitleTenQTransform);
		TenQOBJ->SetModel("world.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "TitleScene", ConeObject_, camera);
		Loader::LoadJsonFileText("Resources", "TitleText", TextObject_);
		Loader::LoadJsonFileNumber("Resources", "TitleNumber", NumberObject_);
		nowStage = 0;
		portal = 0;
	}
	if (ImGui::Button("isDemo")) {
		Remake();
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "DemoScene", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "DemoScene", StarObject_);
		nowStage = 1;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	if (ImGui::Button("isGame")) {
		Remake();
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene1", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene1", StarObject_);
		nowStage = 2;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	if (ImGui::Button("isGame2")) {
		Remake();
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene2", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene2", StarObject_);
		nowStage = 3;
		portal = 0;
		fade->SetTexture(textureHandles[FADE2]);
		fade->StartFadeOut();
		InitStar();
		isPreview = true;
	}
	if (ImGui::Button("isGame3")) {
		Remake();
		TenQOBJ->SetWorldTransform(GameTenQTransform);
		TenQOBJ->SetModel("world2.obj");
		Loader::LoadAllConeJsonFile("Resources", "AllStageCone", "Scene3", ConeObject_, camera);
		Loader::LoadAllStarJsonFile("Resources", "AllStageStar", "Scene3", StarObject_);
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
	if (isTitle == 0) {
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
	if (nowStage == 0) {
		FloorParticle_->Draw(emitter_, emitter_.transform.translate, textureHandles[PARTICLEBLUE], camera, randRange_, false, 0.5f, 0.8f);
		DemoSceneParticle->Draw(
			ParticleEmitter_,
			{ DemoSceneWorldTransformPa.translation_.x, DemoSceneWorldTransformPa.translation_.y, DemoSceneWorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		Stage1Particle->Draw(
			ParticleEmitter_,
			{ GameSceneWorldTransformPa.translation_.x, GameSceneWorldTransformPa.translation_.y, GameSceneWorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		Stage2Particle->Draw(
			ParticleEmitter_,
			{ GameScene2WorldTransformPa.translation_.x, GameScene2WorldTransformPa.translation_.y, GameScene2WorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		Stage3Particle->Draw(
			ParticleEmitter_,
			{ GameScene3WorldTransformPa.translation_.x, GameScene3WorldTransformPa.translation_.y, GameScene3WorldTransformPa.translation_.z },
			textureHandles[WHITE],
			camera,
			demoRandPro,
			false,
			0.2f,
			0.4f
		);

		TitleFireworkPa->Draw(fireworkEmitter_, fireworkEmitter_.transform.translate, textureHandles[PARTICLESTAR], camera, fireworkRange_, false, 0.1f, 0.6f);
		TitleFireworkPa2->Draw(fireworkEmitter_2, fireworkEmitter_2.transform.translate, textureHandles[PARTICLESTAR], camera, fireworkRange_2, false, 0.1f, 0.6f);

		TitleFireworkPa->CreateFireworkEffect(fireworkEmitter_, fireworkRange_, 1.0f, 2.0f, 1.5f, explosionSound, camera->GetTranslate());
		TitleFireworkPa2->CreateFireworkEffect(fireworkEmitter_2, fireworkRange_2, 1.0f, 2.0f, 1.5f, explosionSound, camera->GetTranslate());
	}
	if (nowStage == 1) {
		DemoSceneDemoParticle->Draw(
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
		TitleSceneParticle->Draw(
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
	for (size_t i = 0; i < StarObject_.size() - 1; i++) {
		StarObject_[i]->isVisible = true;
	}
	if (nowStage == 1) {
		StarCount[0] = 2;
	}
	if (nowStage == 2) {
		StarCount[1] = 3;
	}
	if (nowStage == 3) {
		StarCount[2] = 4;
	}
	if (nowStage == 4) {
		StarCount[3] = 5;
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
	if (sceneTime < 180) {
		ApplyLerp(ConeObject_[17]->worldTransform_.translation_.y, 60.0f, 0.03f);
		ApplyLerp(ConeObject_[18]->worldTransform_.translation_.x, 55.0f, 0.03f);
		MoveTitleNumberObjects(8.5f);
	}
	else if (sceneTime < 360) {
		ApplyLerp(ConeObject_[17]->worldTransform_.translation_.y, -4.0f, 0.03f);
		ApplyLerp(ConeObject_[18]->worldTransform_.translation_.x, -50.0f, 0.03f);
		MoveTitleNumberObjects(7.5f);
	}
}

// メイン処理
void GameScene::UpdateTitleScene(const Vector3& playerPos, int sceneTime) {
	// プレイヤー範囲によるテキスト移動
	if (FirstDemoFlag == false) {
		float targetHeight = IsPlayerInRange(playerPos, -20.0f, 20.0f, -20.0f, 20.0f) ? 1.3f : 0.0f;
		ApplyLerp(TextObject_[6]->worldTransform_.translation_.y, targetHeight, 0.1f);
	}
	// タイトルテキスト移動（GameRoop が不要に）
	ApplyLerp(TextObject_[7]->worldTransform_.translation_.y, 2.0f, 0.1f);

	// シーンタイムに応じた移動
	MoveConeObjects(sceneTime);
}

void GameScene::StarSetting(const Vector3& playerPos) {
	for (size_t i = 0; i < StarObject_.size(); i++) {
		if (StarObject_[i]->isVisible) {
			// オブジェクトの座標とサイズを取得
			Vector3 floorPos = StarObject_[i]->worldTransform_.translation_;
			Vector3 floorSize = StarObject_[i]->worldTransform_.scale_;

			// プレイヤーがオブジェクトに当たっているか判定
			if (playerPos.x >= floorPos.x - floorSize.x &&
				playerPos.x <= floorPos.x + floorSize.x &&
				playerPos.z >= floorPos.z - floorSize.z &&
				playerPos.z <= floorPos.z + floorSize.z &&
				playerPos.y >= floorPos.y + floorSize.y - 5.0f &&
				playerPos.y <= floorPos.y + floorSize.y + 5.0f) {
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
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioStarGetSEhandle_, false, 1.0f);
		StarCount[nowStage - 1]--;
	}
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

	}
	ImGui::Text("isgetstar %d", isGetStar);
	ImGui::Text("time %d%d:%d%d", timer.elapsedTensOfMinutes(),timer.elapsedMinutesOnly(),timer.elapsedTensOfSeconds(),timer.elapsedSecondsOnly());
	ImGui::End();
}

