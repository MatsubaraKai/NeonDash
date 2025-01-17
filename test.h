void TitleScene::Update()
{
	fade->UpdateFade();
	PSOPostEffect* pSOPostEffect = PSOPostEffect::GetInstance();

	// プレイヤーの座標
	Vector3 playerPos = camera->transform_.translate;

	PositionOBJ->worldTransform_.translation_ = playerPos;
	PositionOBJ->worldTransform_.translation_.y = camera->transform_.translate.y - 2.99f;

	if (collider->CheckCollision(camera->transform_.translate, worldTransformPa.translation_, 2.5f, 4.0f, 2.5f, 2.0f)) {
		// 衝突している
		portal++;
		isDemo = true;
		isClear = true;
	}
	else {
		isDemo = false;
	}
	if (collider->CheckCollision(camera->transform_.translate, worldTransformPa1.translation_, 2.5f, 4.0f, 2.5f, 2.0f)) {
		// 衝突している
		portal++;
		isGame = true;
		isClear = true;
	}
	else {
		isGame = false;
	}
	if (collider->CheckCollision(camera->transform_.translate, worldTransformPa2.translation_, 2.5f, 4.0f, 2.5f, 2.0f)) {
		// 衝突している
		portal++;
		isGame2 = true;
		isClear = true;
	}
	else {
		isGame2 = false;
	}
	if (collider->CheckCollision(camera->transform_.translate, worldTransformPa3.translation_, 2.5f, 4.0f, 2.5f, 2.0f)) {
		// 衝突している
		portal++;
		isGame3 = true;
		isClear = true;
	}
	else {
		isGame3 = false;
	}
	if (portal == 1) {
		Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioPortalhandle_, false, 0.1f);
	}
	if (sceneTime == 180) {
		effect = true;
	}
	else {
		effect = false;
	}
	if (sceneTime == 360) {
		effect2 = true;
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
		if (isDemo) {
			SetSceneNo(1);
		}
		else if (isGame) {
			SetSceneNo(2);
		}
		else if (isGame2) {
			SetSceneNo(3);
		}
		else if (isGame3) {
			SetSceneNo(4);
		}
	}

	TenQOBJ->worldTransform_.rotation_.y += 0.0005f;

	//各ステージのクリアタイムのNumModelをSet
	for (int i = 0; i < 4; ++i) {
		std::string modelFileName = std::to_string(DemoTime[i]) + ".obj";
		TitleNumberObject_[indices[i]]->SetModel(modelFileName.c_str());
		std::string modelFileName1 = std::to_string(SCENE1Time[i]) + ".obj";
		TitleNumberObject_[indices[i] + 5]->SetModel(modelFileName1.c_str());
		std::string modelFileName2 = std::to_string(SCENE2Time[i]) + ".obj";
		TitleNumberObject_[indices[i] + 10]->SetModel(modelFileName2.c_str());
		std::string modelFileName3 = std::to_string(SCENE3Time[i]) + ".obj";
		TitleNumberObject_[indices[i] + 15]->SetModel(modelFileName3.c_str());
	}
	//各オブジェクトをカメラに向ける
	for (std::vector<Object3d*>::iterator itr = TitleTextObject_.begin(); itr != TitleTextObject_.end(); itr++) {
		(*itr)->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, (*itr)->worldTransform_.translation_) + 3.14f;
	}
	for (std::vector<Object3d*>::iterator itr = TitleNumberObject_.begin(); itr != TitleNumberObject_.end(); itr++) {
		(*itr)->worldTransform_.rotation_.y = camera->Face2Face(camera->transform_.translate, (*itr)->worldTransform_.translation_) + 3.14f;
	}
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

	for (std::vector<Object3d*>::iterator itr = ConeObject_.begin(); itr != ConeObject_.end(); itr++) {
		(*itr)->Update();
	}
	for (std::vector<Object3d*>::iterator itr = TitleTextObject_.begin(); itr != TitleTextObject_.end(); itr++) {
		(*itr)->Update();
	}
	for (std::vector<Object3d*>::iterator itr = TitleNumberObject_.begin(); itr != TitleNumberObject_.end(); itr++) {
		(*itr)->Update();
	}
	if (isClear == false && isMenu == false) {
		camera->Jump(isOnFloor);
		camera->Move(menucount);
	}
	if (!isFadeInStarted && isClear == true) {
		fade->StartFadeIn();    // FadeInを開始
		isFadeInStarted = true; // フラグを立てて一度だけ実行されるようにする
	}
	camera->Update();
	TenQOBJ->Update();
	PositionOBJ->Update();

	if (playerPos.x >= -20.0f &&
		playerPos.x <= 20.0f &&
		playerPos.z >= -20.0f &&
		playerPos.z <= 20.0f && DemoRoop == false
		) {
		TitleTextObject_[6]->worldTransform_.translation_.y = Lerp(TitleTextObject_[6]->worldTransform_.translation_.y, 1.3f, 0.1f);
	}
	else {
		TitleTextObject_[6]->worldTransform_.translation_.y = Lerp(TitleTextObject_[6]->worldTransform_.translation_.y, 0.0f, 0.1f);
	}
	if (DemoRoop == false) {
		TitleTextObject_[7]->worldTransform_.translation_.y = Lerp(TitleTextObject_[7]->worldTransform_.translation_.y, 2.0f, 0.1f);
	}
	else {
		TitleTextObject_[7]->worldTransform_.translation_.y = Lerp(TitleTextObject_[7]->worldTransform_.translation_.y, 0.0f, 0.1f);
	}
	if (sceneTime1 == 0) {

	}
	if (sceneTime1 < 180) {
		ConeObject_[17]->worldTransform_.translation_.y = Lerp(ConeObject_[17]->worldTransform_.translation_.y, 60.0f, 0.03f);
		ConeObject_[18]->worldTransform_.translation_.x = Lerp(ConeObject_[18]->worldTransform_.translation_.x, 55.0f, 0.03f);
		for (int i = 0; i < 6; i++) {
			TitleTextObject_[i]->worldTransform_.translation_.y = Lerp(TitleTextObject_[i]->worldTransform_.translation_.y, Textlerpindices[i], 0.01f);
		}
		for (std::vector<Object3d*>::iterator itr = TitleNumberObject_.begin(); itr != TitleNumberObject_.end(); itr++) {
			(*itr)->worldTransform_.translation_.y = Lerp((*itr)->worldTransform_.translation_.y, 8.5f, 0.01f);
		}
	}
	if (sceneTime1 > 180 && sceneTime1 < 360) {
		ConeObject_[17]->worldTransform_.translation_.y = Lerp(ConeObject_[17]->worldTransform_.translation_.y, -4.0f, 0.03f);
		ConeObject_[18]->worldTransform_.translation_.x = Lerp(ConeObject_[18]->worldTransform_.translation_.x, -50.0f, 0.03f);

		for (int i = 0; i < 6; i++) {
			TitleTextObject_[i]->worldTransform_.translation_.y = Lerp(TitleTextObject_[i]->worldTransform_.translation_.y, textlerpindices[i], 0.01f);
		}
		for (std::vector<Object3d*>::iterator itr = TitleNumberObject_.begin(); itr != TitleNumberObject_.end(); itr++) {
			(*itr)->worldTransform_.translation_.y = Lerp((*itr)->worldTransform_.translation_.y, 7.5f, 0.01f);
		}
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
	std::string label2 = "JSONTextmodel" + std::to_string(selectedIndex2);
	std::string label3 = "JSONNumbermodel" + std::to_string(selectedIndex3);
	ConeObject_[selectedIndex1]->ModelDebug(label1.c_str());
	TitleTextObject_[selectedIndex2]->ModelDebug(label2.c_str());
	TitleNumberObject_[selectedIndex3]->ModelDebug(label3.c_str());

	TenQOBJ->ModelDebug("TenQ");
	PositionOBJ->ModelDebug("positionOBJ");

	particle->Particledebug("white", worldTransformPa);
	particle1->Particledebug("white1", worldTransformPa1);
	particle2->Particledebug("white2", worldTransformPa2);
	particle3->Particledebug("white3", worldTransformPa3);
	ImGui::Begin("Time");
	ImGui::Text("Time : %f", timer.elapsedSeconds());
	ImGui::Text("Time : %d", timer.elapsedSecondsOnly());//一秒単位
	ImGui::Text("Time : %d", timer.elapsedTensOfSeconds());//十秒単位
	ImGui::Text("Time : %d", timer.elapsedMinutesOnly());//一分単位
	ImGui::Text("Time : %d", timer.elapsedTensOfMinutes());//十分単位
	ImGui::Text("Time : %d%d:%d%d", DemoTime[0], DemoTime[1], DemoTime[2], DemoTime[3]);

	if (ImGui::Button("start")) {
		timer.start();
	}
	if (ImGui::Button("stop")) {
		timer.stop();
	}
	ImGui::End();
	ImGui::Begin("isOnFloor");
	ImGui::SliderInt("Select Model Index1", &selectedIndex1, 0, static_cast<int>(ConeObject_.size()) - 2);
	ImGui::SliderInt("Select Model Index2", &selectedIndex2, 0, static_cast<int>(TitleTextObject_.size()) - 2);
	ImGui::SliderInt("Select Model Index3", &selectedIndex3, 0, static_cast<int>(TitleNumberObject_.size()) - 2);
	ImGui::Text("OnFloor : %d", isOnFloor);
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
	ImGui::Text("roop : %d", TitleRoop);
	ImGui::End();
#endif
}
