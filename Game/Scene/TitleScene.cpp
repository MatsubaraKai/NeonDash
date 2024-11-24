#include "TitleScene.h"
#include "ImGuiCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "mathFunction.h"
#include "PSOPostEffect.h"
#include "Loder.h"
#include "Audio.h"
#include <iostream>
#include <cmath>
#include <DirectXMath.h>

// 初期化関数
void TitleScene::Init()
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
	menu->Init(MENUMEDItextureHandle);
	fade = new Fade();
	fade->Init(FADEtextureHandle);

	postProcess_ = new PostProcess();
	postProcess_->SetCamera(camera);
	postProcess_->Init();
}

// シーン更新関数
void TitleScene::Update() {
	// フェード処理の更新
	fade->UpdateFade();

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

	// TenQOBJの回転更新
	TenQOBJ->worldTransform_.rotation_.y += 0.0005f;

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
	UpdateLerpAnimations(playerPos);

	// オブジェクトの更新処理
	UpdateObjects();

	// カメラ更新
	UpdateCamera();

	// デバッグ情報の表示
#ifdef _DEBUG
	DisplayDebugInfo(playerPos);
#endif
}

// 描画関数
void TitleScene::Draw()
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
void TitleScene::PostDraw()
{
	postProcess_->Draw();
}

// リソース解放関数
void TitleScene::Release() {
	Audio::SoundStopWave(Audio::GetInstance()->GetIXAudio().Get(), AudioBGMhandle_);
	Audio::SoundUnload(AudioBGMhandle_);
}

// ゲーム終了判定関数
int TitleScene::GameClose()
{
	return false;
}

///Refactoring///

///Init///
// テクスチャのロード
void TitleScene::LoadTextures()
{
	WHITEtextureHandle = TextureManager::StoreTexture("Resources/white.png");
	PARTICLERED = TextureManager::StoreTexture("Resources/particlered.png");
	PARTICLESTAR = TextureManager::StoreTexture("Resources/particlestar.png");
	PARTICLEBLUE = TextureManager::StoreTexture("Resources/particleblue.png");
	BLUEtextureHandle = TextureManager::StoreTexture("Resources/blue.png");
	FADEtextureHandle = TextureManager::StoreTexture("Resources/black.png");
	MENUMEDItextureHandle = TextureManager::StoreTexture("Resources/game/menumedi.png");
	MENUHIGHtextureHandle = TextureManager::StoreTexture("Resources/game/menuhigh.png");
	MENULOWtextureHandle = TextureManager::StoreTexture("Resources/game/menulow.png");
	GRIDtextureHandle = TextureManager::StoreTexture("Resources/cian.png");
	CONEtextureHandle = TextureManager::StoreTexture("Resources/game/cone.png");
	TENQtextureHandle = TextureManager::StoreTexture("Resources/game/world.png");
	POSITIONtextureHandle = TextureManager::StoreTexture("Resources/game/position.png");
}

// モデルのロード
void TitleScene::LoadModels()
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
void TitleScene::LoadAudio()
{
	AudioBGMhandle_ = Audio::SoundLoadWave("Resources/game/Audio/BGM.wav");
	AudioPortalhandle_ = Audio::SoundLoadWave("Resources/game/Audio/portal.wav");
	AudioTimeCounthandle_ = Audio::SoundLoadWave("Resources/game/Audio/timecount.wav");
	AudioTimeCount2handle_ = Audio::SoundLoadWave("Resources/game/Audio/timecount2.wav");
}

// 初期化データのセットアップ
void TitleScene::InitializeData()
{
	if (!TitleRoop) {
		Loder::LoadJsonFile2("Resources", "TitleCone", ConeObject_);
		Loder::LoadJsonFileText("Resources", "TitleText", TitleTextObject_);
		Loder::LoadJsonFileNumber("Resources", "TitleNumber", TitleNumberObject_);
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioBGMhandle_, true, 0.05f);
		TitleRoop = true;
	}

	for (size_t i = 0; i < ConeObject_.size() - 1; i++) {
		previousPos[i] = ConeObject_[i]->worldTransform_.translation_;
	}

	TenQOBJ = new Object3d();
	TenQOBJ->Init();
	PositionOBJ = new Object3d();
	PositionOBJ->Init();
	PositionOBJ->SetModel("position.obj");
	TenQTransform.Initialize();
	TenQTransform.scale_ = { -100.0f, 100.0f, 100.0f };
	TenQOBJ->SetWorldTransform(TenQTransform);
	TenQOBJ->SetModel("world.obj");
	worldTransformPa.Initialize();
	worldTransformPa.translation_ = { -25.0f, 1.5f, 12.5f };
	worldTransformPa1.Initialize();
	worldTransformPa1.translation_ = { 25.0f, 1.5f, 12.5f };
	worldTransformPa2.Initialize();
	worldTransformPa2.translation_ = { 25.0f, 1.5f, 0.0f };
	worldTransformPa3.Initialize();
	worldTransformPa3.translation_ = { 25.0f, 1.5f, -12.5f };

	camera->transform_.translate = { 0.0f, 6.0f, -15.0f };
	camera->transform_.rotate = { -0.2f, 0.0f, 0.0f };

	isFadeInStarted = false;
	portal = 0;
	isClear = false;
	isMenu = false;
}

// パーティクルシステムの初期化
void TitleScene::InitializeParticles()
{
	emitter_.transform.scale = { 0.2f, 0.2f, 0.2f };
	emitter_.transform.translate = { 0.0f, -1.0f, 0.0f };
	emitter_.frequency = 0.2f;
	emitter_.count = 8;

	randRange_.rangeX = { -50.0f, 50.0f };
	randRange_.rangeY = { 1.0f, 1.0f };
	randRange_.rangeZ = { -65.0f, 38.0f };

	demoRandPro = { {1.0f, 4.0f}, {1.0f, 4.0f}, {1.0f, 4.0f} };

	ParticleEmitter_.count = 6;
	ParticleEmitter_.frequency = 0.02f;
	ParticleEmitter_.frequencyTime = 0.0f;
	ParticleEmitter_.transform.scale = { 0.5f, 0.5f, 0.5f };

	particleSystem_ = new Engine::Particle();
	particleSystem_->Initialize(emitter_);
	particle = new Engine::Particle();
	particle->Initialize(ParticleEmitter_);
	particle1 = new Engine::Particle();
	particle1->Initialize(ParticleEmitter_);
	particle2 = new Engine::Particle();
	particle2->Initialize(ParticleEmitter_);
	particle3 = new Engine::Particle();
	particle3->Initialize(ParticleEmitter_);
}

///Update///
// ポータル判定
void TitleScene::UpdatePortalCollision(const Vector3& playerPos) {
	isDemo = collider->CheckCollision(playerPos, worldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
	isGame = collider->CheckCollision(playerPos, worldTransformPa1.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
	isGame2 = collider->CheckCollision(playerPos, worldTransformPa2.translation_, 2.5f, 4.0f, 2.5f, 2.0f);
	isGame3 = collider->CheckCollision(playerPos, worldTransformPa3.translation_, 2.5f, 4.0f, 2.5f, 2.0f);

	// ポータルが一度だけ発動する処理
	if (isDemo || isGame || isGame2 || isGame3) {
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
void TitleScene::UpdateEffects() {
	effect = (sceneTime == 180);
	effect2 = (sceneTime == 360);

	if (effect) IPostEffectState::SetEffectNo(kOutlinePurple);
	if (effect2) IPostEffectState::SetEffectNo(kOutlineBlue);

	if (sceneTime >= 360) sceneTime = 0;
	if (sceneTime1 >= 360) sceneTime1 = 0;

	if (!isMenu) {
		sceneTime++;
		sceneTime1++;
	}
}

// シーン遷移処理
void TitleScene::HandleSceneTransition() {
	if (isDemo) SetSceneNo(1);
	else if (isGame) SetSceneNo(2);
	else if (isGame2) SetSceneNo(3);
	else if (isGame3) SetSceneNo(4);
}

// ステージタイムのモデル設定
void TitleScene::UpdateStageTimes() {
	for (int i = 0; i < 4; ++i) {
		TitleNumberObject_[indices[i]]->SetModel((std::to_string(DemoTime[i]) + ".obj").c_str());
		TitleNumberObject_[indices[i] + 5]->SetModel((std::to_string(SCENE1Time[i]) + ".obj").c_str());
		TitleNumberObject_[indices[i] + 10]->SetModel((std::to_string(SCENE2Time[i]) + ".obj").c_str());
		TitleNumberObject_[indices[i] + 15]->SetModel((std::to_string(SCENE3Time[i]) + ".obj").c_str());
	}
}

// 各オブジェクトをカメラに向ける
void TitleScene::AlignObjectsToCamera() {
	for (auto& obj : TitleTextObject_) {
		obj->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, obj->worldTransform_.translation_) + 3.14f;
	}
	for (auto& obj : TitleNumberObject_) {
		obj->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, obj->worldTransform_.translation_) + 3.14f;
	}
}

// ゲームパッド入力処理
void TitleScene::HandleGamePadInput() {
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(joyState)) {
		HandleStartButton(joyState);
		if (isMenu) HandleMenuNavigation(joyState);
		else UpdateCameraFOV(joyState);
	}
}

// STARTボタンの入力を処理
void TitleScene::HandleStartButton(const XINPUT_STATE& joyState) {
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
void TitleScene::HandleMenuNavigation(const XINPUT_STATE& joyState) {
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

	menu->ChangeTex((menucount == 0) ? MENULOWtextureHandle :
		(menucount == 1) ? MENUMEDItextureHandle : MENUHIGHtextureHandle);
}

// 床の移動アニメーションを更新
void TitleScene::UpdateLerpAnimations(const Vector3& playerPos) {
	// プレイヤーが指定範囲内にいる場合の処理
	if (playerPos.x >= -20.0f && playerPos.x <= 20.0f &&
		playerPos.z >= -20.0f && playerPos.z <= 20.0f && DemoRoop == false) {
		// TitleTextObject_[6]を徐々に高さ 1.3 に
		TitleTextObject_[6]->worldTransform_.translation_.y =
			Lerp(TitleTextObject_[6]->worldTransform_.translation_.y, 1.3f, 0.1f);
	}
	else {
		// TitleTextObject_[6]を徐々に高さ 0.0 に戻す
		TitleTextObject_[6]->worldTransform_.translation_.y =
			Lerp(TitleTextObject_[6]->worldTransform_.translation_.y, 0.0f, 0.1f);
	}

	// DemoRoop に応じた TitleTextObject_[7] の高さ変更
	if (DemoRoop == false) {
		TitleTextObject_[7]->worldTransform_.translation_.y =
			Lerp(TitleTextObject_[7]->worldTransform_.translation_.y, 2.0f, 0.1f);
	}
	else {
		TitleTextObject_[7]->worldTransform_.translation_.y =
			Lerp(TitleTextObject_[7]->worldTransform_.translation_.y, 0.0f, 0.1f);
	}

	// シーンタイムによるアニメーション
	if (sceneTime1 < 180) {
		// ConeObject_[17]と[18]の座標をそれぞれの目標位置に向けて移動
		ConeObject_[17]->worldTransform_.translation_.y =
			Lerp(ConeObject_[17]->worldTransform_.translation_.y, 60.0f, 0.03f);
		ConeObject_[18]->worldTransform_.translation_.x =
			Lerp(ConeObject_[18]->worldTransform_.translation_.x, 55.0f, 0.03f);

		// TitleTextObject_ の各インデックスを目標の高さに移動
		for (int i = 0; i < 6; i++) {
			TitleTextObject_[i]->worldTransform_.translation_.y =
				Lerp(TitleTextObject_[i]->worldTransform_.translation_.y, Textlerpindices[i], 0.01f);
		}

		// TitleNumberObject_ の各オブジェクトを高さ 8.5 に移動
		for (auto& obj : TitleNumberObject_) {
			obj->worldTransform_.translation_.y =
				Lerp(obj->worldTransform_.translation_.y, 8.5f, 0.01f);
		}
	}
	else if (sceneTime1 > 180 && sceneTime1 < 360) {
		// ConeObject_[17]と[18]を別の位置に移動
		ConeObject_[17]->worldTransform_.translation_.y =
			Lerp(ConeObject_[17]->worldTransform_.translation_.y, -4.0f, 0.03f);
		ConeObject_[18]->worldTransform_.translation_.x =
			Lerp(ConeObject_[18]->worldTransform_.translation_.x, -50.0f, 0.03f);

		// TitleTextObject_ を別の目標位置に移動
		for (int i = 0; i < 6; i++) {
			TitleTextObject_[i]->worldTransform_.translation_.y =
				Lerp(TitleTextObject_[i]->worldTransform_.translation_.y, textlerpindices[i], 0.01f);
		}

		// TitleNumberObject_ の各オブジェクトを高さ 7.5 に移動
		for (auto& obj : TitleNumberObject_) {
			obj->worldTransform_.translation_.y =
				Lerp(obj->worldTransform_.translation_.y, 7.5f, 0.01f);
		}
	}
}

// 床とのインタラクション処理
void TitleScene::UpdateFloorInteraction()
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
void TitleScene::UpdateCamera() {
	// プレイヤーが床の上にいるかどうかでジャンプ処理を実行
	if (isClear == false && isMenu == false) {
		camera->Jump(isOnFloor);
		camera->Move(menucount);
	}
	
	// フェードイン中であればカメラの移動を制御
	if (!isFadeInStarted && isClear == true) {
		fade->StartFadeIn();    // フェードインを開始
		isFadeInStarted = true; // フラグを立て、一度だけ実行
	}

	// カメラの更新処理を呼び出す
	camera->Update();
}

// FOV（視野角）の更新
void TitleScene::UpdateCameraFOV(const XINPUT_STATE& joyState) {
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
	camera->SetFOV(Lerp(camera->fovY_, desiredFOV, 0.1f));
}

// プレイヤーと床の衝突処理
void TitleScene::UpdatePlayerFloorCollision(const Vector3& playerPos) {
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
void TitleScene::UpdateObjects() {
	for (auto& obj : ConeObject_) obj->Update();
	for (auto& obj : TitleTextObject_) obj->Update();
	for (auto& obj : TitleNumberObject_) obj->Update();

	TenQOBJ->Update();
	PositionOBJ->Update();
}

// デバッグ情報の表示
void TitleScene::DisplayDebugInfo(const Vector3& playerPos) {
	ImGui::Begin("Debug Info");
	ImGui::Text("Player Position: X=%f, Y=%f, Z=%f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::Text("On Floor: %d", isOnFloor);
	ImGui::End();
	ImGui::Begin("isOnFloor");
	ImGui::SliderInt("Select Model Index1", &selectedIndex1, 0, static_cast<int>(ConeObject_.size()) - 2);
	ImGui::Text("OnFloor : %d", isOnFloor);
	ImGui::Text("Player Pos : %f %f %f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::End();

	for (size_t i = 0; i < ConeObject_.size() - 1; i++) {
		float previousFloorHeight = playerPos.y; // 初期化しておく
		// オブジェクトの座標とサイズを取得
		Vector3 floorPos = ConeObject_[i]->worldTransform_.translation_;
		Vector3 floorSize = ConeObject_[i]->worldTransform_.scale_;
		std::string label = "JSONmodel" + std::to_string(i);
#ifdef _DEBUG

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
#endif
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
}

///Draw///
// コーンオブジェクトの描画
void TitleScene::DrawConeObjects()
{
	for (auto& cone : ConeObject_) {
		if (cone->isVisible) {
			cone->Draw(CONEtextureHandle, camera);
		}
	}
}

// タイトルテキストの描画
void TitleScene::DrawTitleTextObjects()
{
	for (size_t i = 0; i < TitleTextObject_.size(); ++i) {
		auto& textObj = TitleTextObject_[i];
		if (i == 0 || i == 1) {
			textObj->Draw(BLUEtextureHandle, camera); // 特定のオブジェクトは異なるテクスチャで描画
		}
		else {
			textObj->Draw(GRIDtextureHandle, camera);
		}
	}
}

// タイトルナンバーの描画
void TitleScene::DrawTitleNumberObjects()
{
	for (auto& numberObj : TitleNumberObject_) {
		numberObj->Draw(GRIDtextureHandle, camera);
	}
}

// 特殊オブジェクトの描画
void TitleScene::DrawSpecialObjects()
{
	TenQOBJ->Draw(TENQtextureHandle, camera);
	if (menuposition) {
		PositionOBJ->Draw(POSITIONtextureHandle, camera);
	}
}

// パーティクルの描画
void TitleScene::DrawParticles()
{
	particleSystem_->Draw(emitter_, emitter_.transform.translate, PARTICLEBLUE, camera, randRange_, false, 0.5f, 0.8f);

	particle->Draw(
		ParticleEmitter_,
		{ worldTransformPa.translation_.x, worldTransformPa.translation_.y, worldTransformPa.translation_.z },
		WHITEtextureHandle,
		camera,
		demoRandPro,
		false,
		0.2f,
		0.4f
	);

	particle1->Draw(
		ParticleEmitter_,
		{ worldTransformPa1.translation_.x, worldTransformPa1.translation_.y, worldTransformPa1.translation_.z },
		WHITEtextureHandle,
		camera,
		demoRandPro,
		false,
		0.2f,
		0.4f
	);

	particle2->Draw(
		ParticleEmitter_,
		{ worldTransformPa2.translation_.x, worldTransformPa2.translation_.y, worldTransformPa2.translation_.z },
		WHITEtextureHandle,
		camera,
		demoRandPro,
		false,
		0.2f,
		0.4f
	);

	particle3->Draw(
		ParticleEmitter_,
		{ worldTransformPa3.translation_.x, worldTransformPa3.translation_.y, worldTransformPa3.translation_.z },
		WHITEtextureHandle,
		camera,
		demoRandPro,
		false,
		0.2f,
		0.4f
	);
}


