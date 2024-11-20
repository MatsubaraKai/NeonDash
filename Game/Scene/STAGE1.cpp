#include "STAGE1.h"
#include "ImGuiCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Loder.h"
#include "PSOPostEffect.h"
#include "Audio.h"

#include <DirectXMath.h>
#include "Vector3.h"

void STAGE1::Init()
{
	camera = new Camera;
	camera->Initialize();
	input = Input::GetInstance();
	WHITEtextureHandle = TextureManager::StoreTexture("Resources/white.png");
	BLUEtextureHandle = TextureManager::StoreTexture("Resources/blue.png");
	MENUMEDItextureHandle = TextureManager::StoreTexture("Resources/game/menumedi.png");
	MENUHIGHtextureHandle = TextureManager::StoreTexture("Resources/game/menuhigh.png");
	MENULOWtextureHandle = TextureManager::StoreTexture("Resources/game/menulow.png");
	CONEtextureHandle = TextureManager::StoreTexture("Resources/game/cone.png");
	TENQtextureHandle = TextureManager::StoreTexture("Resources/game/world2.png");
	FADEtextureHandle = TextureManager::StoreTexture("Resources/black.png");
	FADE2textureHandle = TextureManager::StoreTexture("Resources/black2.png");
	FADE3textureHandle = TextureManager::StoreTexture("Resources/black3.png");
	GRIDtextureHandle = TextureManager::StoreTexture("Resources/cian.png");
	STARtextureHandle = TextureManager::StoreTexture("Resources/game/star.png");
	AudioStarGetSEhandle_ = Audio::SoundLoadWave("Resources/game/Audio/GetSE.wav");
	AudioPortalhandle_ = Audio::SoundLoadWave("Resources/game/Audio/portal.wav");
	AudioTimeCounthandle_ = Audio::SoundLoadWave("Resources/game/Audio/timecount.wav");
	AudioTimeCount2handle_ = Audio::SoundLoadWave("Resources/game/Audio/timecount2.wav");
	POSITIONtextureHandle = TextureManager::StoreTexture("Resources/game/position.png");

	if (GameRoop == false) {
		Loder::LoadJsonFile2("Resources", "GameCone", ConeObject_);
		Loder::LoadJsonFile2("Resources", "GameStar", StarObject_);
		GameRoop = true;
	}
	for (size_t i = 0; i < ConeObject_.size() - 1; i++) {
		previousPos[i] = ConeObject_[i]->worldTransform_.translation_;
	}
	for (size_t i = 0; i < StarObject_.size() - 1; i++) {
		StarObject_[i]->isVisible = true;
	}
	postProcess_ = new PostProcess();
	postProcess_->SetCamera(camera);
	postProcess_->Init();
	TenQOBJ = new Object3d();
	TenQOBJ->Init();
	PositionOBJ = new Object3d();
	PositionOBJ->Init();
	TextOBJ = new Object3d();
	TextOBJ->Init();
	Number = new Object3d();
	Number->Init();
	starCount = 3;
	portal = 0;
	isFadeInStarted = false;	

	worldTransformPa.Initialize();
	worldTransformPa2.Initialize();
	TenQTransform.Initialize();

	worldTransformPa.translation_ = { -2.5f,55.5f,220.0f };
	worldTransformPa2.translation_ = { -20.0f,1.5f,-17.5f };

	TenQTransform.translation_.y = 2200.0f;
	TenQTransform.translation_.z = 700.0f;
	TenQTransform.scale_.x = -10.0f;
	TenQTransform.scale_.y = 10.0f;
	TenQTransform.scale_.z = 10.0f;
	TenQOBJ->SetWorldTransform(TenQTransform);
	TextOBJ->worldTransform_.translation_ = { -17.5f,7.0f,-15.0f };
	camera->transform_.translate = { 0.0f,15.0f,-15.0f };
	camera->transform_.rotate = { -0.2f, 0.0f, 0.0f };


	Number->worldTransform_.translation_ = { 0.0f,61.0f,222.5f };
	Number->worldTransform_.scale_ = { 2.0f,2.0f,2.0f };
	TenQOBJ->SetModel("world2.obj");
	TextOBJ->SetModel("text7.obj");
	PositionOBJ->SetModel("position.obj");
	particle = new Particle();				
	particle2 = new Particle();
	demoRandPro = {
		{1.0f,4.0f},
		{1.0f,4.0f},
		{1.0f,4.0f}
	};

	ParticleEmitter_.count = 6;
	ParticleEmitter_.frequency = 0.02f;
	ParticleEmitter_.frequencyTime = 0.0f;
	ParticleEmitter_.transform.scale = { 0.5f,0.5f,0.5f };
	particle->Initialize(ParticleEmitter_);
	particle2->Initialize(ParticleEmitter_);
	isMenu = false;
	isPreview = true;

	menu = new Menu();							 
	menu->Init(MENUMEDItextureHandle);
	fade = new Fade();
	fade->Init(FADE2textureHandle);
	fade->StartFadeOut();
}

void STAGE1::Update()
{
	if (previousIsPreview && !isPreview) {
		timer.start();
	}
	previousIsPreview = isPreview;
	fade->UpdateFade();
	PSOPostEffect* pSOPostEffect = PSOPostEffect::GetInstance();
	// プレイヤーの座標
	std::string modelFileName = std::to_string(starCount) + ".obj";
	Number->SetModel(modelFileName.c_str());
	Vector3 playerPos = camera->transform_.translate;
	PositionOBJ->worldTransform_.translation_ = playerPos;
	PositionOBJ->worldTransform_.translation_.y = camera->transform_.translate.y - 2.99f;


	if (collider->CheckCollision(camera->transform_.translate, worldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f) && starCount == 0) {
		// 衝突している
		if (SCENE1Time[4] == 0) {//最初は0なので一回通す
			SCENE1Time[0] = timer.elapsedTensOfMinutes();
			SCENE1Time[1] = timer.elapsedMinutesOnly();
			SCENE1Time[2] = timer.elapsedTensOfSeconds();
			SCENE1Time[3] = timer.elapsedSecondsOnly();
			SCENE1Time[4] = static_cast<int>(timer.elapsedSeconds());
		}
		if (static_cast<int>(timer.elapsedSeconds()) < SCENE1Time[4]) {//前回の記録より早かったら記録
			SCENE1Time[0] = timer.elapsedTensOfMinutes();
			SCENE1Time[1] = timer.elapsedMinutesOnly();
			SCENE1Time[2] = timer.elapsedTensOfSeconds();
			SCENE1Time[3] = timer.elapsedSecondsOnly();
			SCENE1Time[4] = static_cast<int>(timer.elapsedSeconds());
		}
		portal++;
		timer.stop();//タイマー止める
		isClear = true;
	}
	else {
		isClear = false;

	}
	if (collider->CheckCollision(camera->transform_.translate, worldTransformPa2.translation_, 2.5f, 4.0f, 2.5f, 2.0f)) {
		// 衝突している
		portal++;
		isTitle = true;
		isClear = true;
	}
	else {
		isTitle = false;
	}
	if (portal == 1) {
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioPortalhandle_, false, 0.1f);
	}
	if ((sceneTime == 60 || sceneTime == 120 || sceneTime == 240 || sceneTime == 300) && isMenu == false) {
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioTimeCounthandle_, false, 1.0f);
	}
	if (sceneTime == 180 && isMenu == false) {
		effect = true;
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioTimeCount2handle_, false, 1.0f);

	}
	else {
		effect = false;
	}
	if (sceneTime == 360 && isMenu == false) {
		effect2 = true;
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioTimeCount2handle_, false, 1.0f);
	}
	else {
		effect2 = false;
	}
	if (effect == true) {
		IPostEffectState::SetEffectNo(kOutlinePurple);
	}
	if (effect2 == true) {
		IPostEffectState::SetEffectNo(kOutlineBlue);
	}
	if (sceneTime >= 360 || sceneTime1 >= 360) {
		sceneTime = 0;
		sceneTime1 = 0;
	}
	if (fade->IsFadeOutComplete()) {
		SetSceneNo(0);
	}
	Number->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, Number->worldTransform_.translation_) + 3.14f;
	TextOBJ->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, TextOBJ->worldTransform_.translation_) + 3.14f;
	TenQOBJ->worldTransform_.rotation_.x += 0.001f;
	
	// ゲームパッドの状態取得
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(joyState))
	{
		// START ボタンが押された場合
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
			// ボタンが押された状態で、前回押されていなかった場合のみトグル
			if (!startButtonPressed) {
				isMenu = !isMenu;           // isMenu の値を反転させる
				startButtonPressed = true;   // ボタンが押された状態にする
			}
		}
		else {
			// ボタンが離されたらフラグをリセット
			startButtonPressed = false;
		}
		if (isMenu) {
			// 前回のボタンの状態を保持する変数を用意
			static WORD previousButtons = 0;

			// 現在のボタンの状態を取得
			WORD currentButtons = joyState.Gamepad.wButtons;

			// 左肩ボタンが「押された瞬間」を検出
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
			// 前回のボタンの状態を更新
			previousButtons = currentButtons;
			if (menucount == 0) {
				menu->ChangeTex(MENULOWtextureHandle);
			}
			if (menucount == 1) {
				menu->ChangeTex(MENUMEDItextureHandle);
			}
			if (menucount == 2) {
				menu->ChangeTex(MENUHIGHtextureHandle);
			}
		}
		// 左スティックによる移動
		Vector3 moveLeftStick = { 0, 0, 0 };
		Vector3 move = { 0.0f, 0.0f, 0.0f };
		const float leftStickDeadZone = 0.2f;
		if (std::abs(joyState.Gamepad.sThumbLX) > leftStickDeadZone * SHRT_MAX ||
			std::abs(joyState.Gamepad.sThumbLY) > leftStickDeadZone * SHRT_MAX)
		{
			moveLeftStick = {
				(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
				0.0f,
				(float)joyState.Gamepad.sThumbLY / SHRT_MAX
			};

			float inputMagnitude = Length(moveLeftStick);
			if (inputMagnitude > leftStickDeadZone)
			{
				moveLeftStick.x *= 0.004f;
				moveLeftStick.z *= 0.004f;
			}
		}
		float desiredFOV = camera->fovY_;  // 現在のFOVを基準にする

		if (moveLeftStick.z > 0.00001f) {  // スティックがある程度前に倒されたとき
			desiredFOV = 1.0f;  // 前進時のFOV
		}
		else {
			desiredFOV = 0.8f;  // デフォルトのFOV
		}

		// 現在のFOVと目的のFOVが異なる場合のみLerpを行う
		if (camera->fovY_ != desiredFOV) {
			camera->SetFOV(Lerp(camera->fovY_, desiredFOV, 0.1f));
		}
		// カメラの向きに基づく移動方向の調整
		if (moveLeftStick.x != 0.0f || moveLeftStick.z != 0.0f)
		{
			// 移動ベクトルをカメラの向きに合わせる
			float cosY = cosf(camera->transform_.rotate.y);
			float sinY = sinf(camera->transform_.rotate.y);
			move = {
				moveLeftStick.x * cosY + moveLeftStick.z * sinY,
				0.0f,
				moveLeftStick.z * cosY - moveLeftStick.x * sinY
			};
			TenQOBJ->worldTransform_.rotation_.x += move.z;
		}
	}
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

				// 床の上昇分を計算
				float floorHeightChange = floorPos.y + floorSize.y - previousFloorHeight;
				camera->transform_.translate.y = playerPos.y += floorHeightChange + 3.0f;  // プレイヤーの高さを更新
				previousFloorHeight = floorPos.y + floorSize.y; // 次フレームのために保存

				// x軸、z軸の移動分を計算してプレイヤーに反映
				Vector3 floorMovement;
				floorMovement.x = floorPos.x - previousPos[i].x;
				floorMovement.z = floorPos.z - previousPos[i].z;

				camera->transform_.translate.x += floorMovement.x;
				camera->transform_.translate.z += floorMovement.z;

				// 現在のオブジェクト位置を次のフレームで使用するため保存
				previousPos[i] = floorPos;

				isOnFloor = true;
				break;  // どれかのオブジェクト上にいる場合は判定を終了
			}
			else {
				isOnFloor = false;
				previousPos[i] = floorPos;

			}
	}

	for (size_t i = 0; i < StarObject_.size() - 1; i++) {
		if (StarObject_[i]->isVisible) {
			// オブジェクトの座標とサイズを取得
			Vector3 floorPos = StarObject_[i]->worldTransform_.translation_;
			Vector3 floorSize = StarObject_[i]->worldTransform_.scale_;

			// プレイヤーがオブジェクトに当たっているか判定
			if (playerPos.x >= floorPos.x - floorSize.x &&
				playerPos.x <= floorPos.x + floorSize.x &&
				playerPos.z >= floorPos.z - floorSize.z &&
				playerPos.z <= floorPos.z + floorSize.z &&
				playerPos.y >= floorPos.y + floorSize.y - 3.0f &&
				playerPos.y <= floorPos.y + floorSize.y + 3.0f) {
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
		starCount--;

	}
	for (std::vector<Object3d*>::iterator itr1 = ConeObject_.begin(); itr1 != ConeObject_.end(); itr1++) {
			(*itr1)->Update();
	}
	for (std::vector<Object3d*>::iterator itr2 = StarObject_.begin(); itr2 != StarObject_.end(); itr2++) {
		if ((*itr2)->isVisible) {
			(*itr2)->Update();
			(*itr2)->worldTransform_.rotation_.y += 0.02f;
		}
	}
	if (isPreview == true) {
		camera->StagePreview(stageCenter, stageRadius, rotationSpeed, angleX, isPreview);
		if (camera->isEasing == true) {
			fade->SetAlpha(0.0f);
		}
	}
	else
	{
		camera->isEasing = false;
	}
	if (isClear == false && isMenu == false && isPreview == false) {
		camera->Jump(isOnFloor);
		camera->Move(menucount);
	}
	if (!isFadeInStarted && isClear == true) {
		if (collider->CheckCollision(camera->transform_.translate, worldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f) && starCount == 0) {
			fade->SetTexture(FADE3textureHandle);
		}
		else {
			fade->SetTexture(FADEtextureHandle);
		}
		fade->StartFadeIn();    // FadeInを開始
		isFadeInStarted = true; // フラグを立てて一度だけ実行されるようにする
	}
	camera->Update();
	TenQOBJ->Update();
	PositionOBJ->Update();
	Number->Update();
	TextOBJ->Update();

	
	if (sceneTime1 == 0) {

	}
	if (sceneTime1 < 180 && isMenu == false) {
		TextOBJ->worldTransform_.translation_.y = Lerp(TextOBJ->worldTransform_.translation_.y, 7.5f, 0.01f);
		ConeObject_[1]->worldTransform_.translation_.z = Lerp(ConeObject_[1]->worldTransform_.translation_.z, 0.0f, 0.018f);
		ConeObject_[3]->worldTransform_.translation_.y = Lerp(ConeObject_[3]->worldTransform_.translation_.y, -7.0f, 0.018f);
		StarObject_[1]->worldTransform_.translation_.y = Lerp(StarObject_[1]->worldTransform_.translation_.y, 1.0f, 0.018f);
	}
	if (sceneTime1 > 180 && sceneTime1 < 360 && isMenu == false) {
		TextOBJ->worldTransform_.translation_.y = Lerp(TextOBJ->worldTransform_.translation_.y, 6.5f, 0.01f);
		ConeObject_[1]->worldTransform_.translation_.z = Lerp(ConeObject_[1]->worldTransform_.translation_.z, 100.0f, 0.018f);
		ConeObject_[3]->worldTransform_.translation_.y = Lerp(ConeObject_[3]->worldTransform_.translation_.y, 55.0f, 0.018f);
		StarObject_[1]->worldTransform_.translation_.y = Lerp(StarObject_[1]->worldTransform_.translation_.y, 63.0f, 0.018f);

	

	}
	
	if (effectFlag == true && isMenu == false) {
		sceneTime++;
	}
	if (isMenu == false) {
		sceneTime1++;
	}
	///////////////Debug///////////////
#ifdef _DEBUG
	camera->CameraDebug();
	// 選択されたインデックスに応じたモデルのデバッグを実行
	std::string label1 = "JSONConemodel" + std::to_string(selectedIndex1);
	std::string label2 = "JSONStarmodel" + std::to_string(selectedIndex2);
	ConeObject_[selectedIndex1]->ModelDebug(label1.c_str());
	StarObject_[selectedIndex2]->ModelDebug(label2.c_str());

	TenQOBJ->ModelDebug("TenQ");
	PositionOBJ->ModelDebug("positionOBJ");
	Number->ModelDebug("num");
	TextOBJ->ModelDebug("text7");

	particle->Particledebug("white", worldTransformPa);
	particle2->Particledebug("white2", worldTransformPa2);
	ImGui::Begin("Time");
	ImGui::Text("Time : %f", timer.elapsedSeconds());
	ImGui::Text("Time : %d", timer.elapsedSecondsOnly());
	ImGui::Text("Time : %d", timer.elapsedTensOfSeconds());
	ImGui::Text("Time : %d", timer.elapsedMinutesOnly());
	ImGui::Text("Time : %d", timer.elapsedTensOfMinutes());

	if (ImGui::Button("start")) {
		timer.start();
	}
	if (ImGui::Button("stop")) {
		timer.stop();
	}

	ImGui::End();
	ImGui::Begin("isOnFloor");
	ImGui::SliderInt("Select Model Index1", &selectedIndex1, 0, static_cast<int>(ConeObject_.size()) - 2);
	ImGui::SliderInt("Select Model Index2", &selectedIndex2, 0, static_cast<int>(StarObject_.size()) - 2);
	ImGui::Text("starcount: %d", starCount);
	ImGui::Text("OnFloor : %d", isOnFloor);
	ImGui::Text("GetStar : %d", isGetStar);
	ImGui::Text("Player Pos : %f %f %f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::End();
	ImGui::Begin("color", nullptr, ImGuiWindowFlags_MenuBar);
	//float color[4] = { fade->material.color.x,fade->material.color.y,fade->material.color.z,fade->material.color.w };
	//ImGui::DragFloat4("color", color, 0.01f);
	//fade->material.color = { color[0],color[1],color[2],color[3] };
	//いつか使う用に↓
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save")) {

			}
			if (ImGui::MenuItem("Load")) {

			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();
	ImGui::Begin("Imgui");
	ImGui::Checkbox("EffectFlag", &effectFlag);
	ImGui::Text("Now Scene : %d", sceneNo);
	ImGui::Text("roop : %d", GameRoop);
	ImGui::End();
#endif
}
void STAGE1::Draw()
{
	for (std::vector<Object3d*>::iterator itr1 = ConeObject_.begin(); itr1 != ConeObject_.end(); itr1++) {
			(*itr1)->Draw(CONEtextureHandle, camera);
	}
	for (std::vector<Object3d*>::iterator itr2 = StarObject_.begin(); itr2 != StarObject_.end(); itr2++) {
		if ((*itr2)->isVisible) {
			(*itr2)->Draw(STARtextureHandle, camera);
		}
	}
	TenQOBJ->Draw(TENQtextureHandle, camera);
	if (menuposition == true) {
		PositionOBJ->Draw(POSITIONtextureHandle, camera);
	}
	particle->Draw(ParticleEmitter_, { worldTransformPa.translation_.x,worldTransformPa.translation_.y,worldTransformPa.translation_.z }, WHITEtextureHandle, camera, demoRandPro, false, 0.2f, 0.4f);
	particle2->Draw(ParticleEmitter_, { worldTransformPa2.translation_.x,worldTransformPa2.translation_.y,worldTransformPa2.translation_.z }, WHITEtextureHandle, camera, demoRandPro, false, 0.2f, 0.4f);
	Number->Draw(GRIDtextureHandle, camera);
	TextOBJ->Draw(GRIDtextureHandle, camera);
	if (isMenu) {
		menu->Draw();
	}
	fade->Draw();
}

void STAGE1::PostDraw()
{
	postProcess_->Draw();
}

void STAGE1::Release() {
	delete camera;
	delete postProcess_;
	delete TenQOBJ;
	delete PositionOBJ;
	delete TextOBJ;
	delete Number;
	delete particle;
	delete particle2;
	delete menu;
	delete fade;
}
// ゲームを終了
int STAGE1::GameClose()
{
	return false;
}