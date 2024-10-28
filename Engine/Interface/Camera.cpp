#include "Camera.h"
#include <algorithm>

void Camera::Initialize() {
    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-80.0f} };
    //WinAPI* sWinAPI = WinAPI::GetInstance();

    cameraMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(cameraMatrix_);
    projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, asepectRatio_, nearClip_, farClip_);
    //worldCameraMatrix = Multiply(worldmatrix, Multiply(viewMatrix, projectionMatrix));
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::Update() {
    cameraMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(cameraMatrix_);
    projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, asepectRatio_, nearClip_, farClip_);
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}
void Camera::CameraDebug()
{
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.0f, 0.7f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.1f, 0.0f, 0.3f, 0.5f));
    ImGui::Begin("camera");
    ImGui::DragFloat("FOV", &fovY_, 0.01f);
    float translate[3] = { transform_.translate.x,transform_.translate.y,transform_.translate.z };
    ImGui::DragFloat3("transform", translate, 0.01f);
    float rotate[3] = { transform_.rotate.x,transform_.rotate.y,transform_.rotate.z };
    ImGui::DragFloat3("rotate", rotate, 0.01f);
    float scale[3] = { transform_.scale.x, transform_.scale.y, transform_.scale.z };
    ImGui::DragFloat3("scale", scale, 0.01f);
    ImGui::Text("jumpspeed : %f", jumpVelocity);


    transform_.translate = { translate[0],translate[1],translate[2] };
    transform_.rotate = { rotate[0],rotate[1],rotate[2] };
    transform_.scale = { scale[0],scale[1],scale[2] };

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void Camera::Move(int menucount)
{
    // ゲームパッドの処理
    HandleGamepadMovement(menucount);
}

void Camera::HandleGamepadMovement(int menucount)
{
    if (isDie == false) {
        XINPUT_STATE joyState;
        if (Input::GetInstance()->GetJoystickState(joyState))
        {

            // 左スティックによる移動
            Vector3 moveLeftStick = { 0, 0, 0 };
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
                    moveLeftStick.x *= PlayerSpeed;
                    moveLeftStick.z *= PlayerSpeed;
                }

            }

            // カメラの向きに基づく移動方向の調整
            if (moveLeftStick.x != 0.0f || moveLeftStick.z != 0.0f)
            {
                float cosY = cosf(transform_.rotate.y);
                float sinY = sinf(transform_.rotate.y);
                Vector3 move = {
                    moveLeftStick.x * cosY + moveLeftStick.z * sinY,
                    0.0f,
                    moveLeftStick.z * cosY - moveLeftStick.x * sinY
                };
                transform_.translate.x += move.x;
                transform_.translate.z += move.z;
            }

            // 右スティックによる視野の移動
            HandleRightStick(joyState, menucount);
        }
    }
}

void Camera::SetFOV(float fovY) {
    fovY_ = fovY;
}

void Camera::HandleRightStick(const XINPUT_STATE& joyState, int menucount)
{
    const float rightStickDeadZone = 0.1f;
    if (std::abs(joyState.Gamepad.sThumbRX) > rightStickDeadZone * SHRT_MAX ||
        std::abs(joyState.Gamepad.sThumbRY) > rightStickDeadZone * SHRT_MAX)
    {
        float rightStickX = (float)joyState.Gamepad.sThumbRX / SHRT_MAX * lookSensitivity;
        float rightStickY = (float)joyState.Gamepad.sThumbRY / SHRT_MAX * lookSensitivity;

        transform_.rotate.y += rightStickX;  // Yaw (水平回転)
        transform_.rotate.x -= rightStickY;  // Pitch (垂直回転)

        // ピッチの制限
        const float maxPitch = 45.0f * (float)std::numbers::pi / 180.0f;
        const float minPitch = -45.0f * (float)std::numbers::pi / 180.0f;
        transform_.rotate.x = std::clamp(transform_.rotate.x, minPitch, maxPitch);
    }
    if (menucount == 0) {
        lookSensitivity = 0.02f;
    }
    if (menucount == 1) {
        lookSensitivity = 0.03f;
    }
    if (menucount == 2) {
        lookSensitivity = 0.06f;
    }
}

void Camera::Jump(bool isOnFloor)
{
    static float easingProgress = 0.0f;

    if (isDie) {
        if (easingProgress == 0.0f) {
            Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioPortalhandle_, false, 0.30f);
        }
        jumpVelocity = 0.0f;
        isJumping = false;
        easingProgress += 0.0001f;  // イージング進行度

        transform_.translate.x = Lerp2(transform_.translate.x, 0.0f, easingProgress);
        transform_.translate.y = Lerp2(transform_.translate.y, 7.0f, easingProgress);
        transform_.translate.z = Lerp2(transform_.translate.z, -15.0f, easingProgress);
        transform_.rotate.x = Lerp2(transform_.rotate.x, -0.2f, easingProgress);
        transform_.rotate.y = Lerp2(transform_.rotate.y, 0.0f, easingProgress);
        transform_.rotate.z = Lerp2(transform_.rotate.z, 0.0f, easingProgress);
    }
    if (easingProgress >= 1.0f) {
        easingProgress = 0.0f;
        isDie = false;
    }
    if (fabs(transform_.translate.x - 0.0f) <= 0.5f &&
        fabs(transform_.translate.y - 7.0f) <= 0.5f &&
        fabs(transform_.translate.z + 15.0f) <= 0.5f) {
        easingProgress = 0.0f;
        isDie = false;
    }
    if (isJumping) {
        // ジャンプ中の処理
        transform_.translate.y += jumpVelocity;
        jumpVelocity += Gravity;

        if (transform_.translate.y <= -10.0f) {
            // 落下しすぎた場合のリセット処理
            jumpVelocity = 0.0f;
            isDie = true;
            isJumping = false;
        }
    }
    else if (!isOnFloor) {
        // 地面にいない場合の処理
        transform_.translate.y += jumpVelocity;
        jumpVelocity += Gravity;
        if (transform_.translate.y <= -10.0f) {
            // 落下しすぎた場合のリセット処理
            jumpVelocity = 0.0f;
            isDie = true;
            isJumping = false;
        }
    }
    if (isOnFloor) {
        // 地面にいる場合
         // 前フレームと同じ jumpVelocity であれば 0 に設定
        if (jumpVelocity == previousJumpVelocity) {
            jumpVelocity = 0.0f;
        }
        isJumping = false;
    }
    // 前フレームの jumpVelocity を保存
    previousJumpVelocity = jumpVelocity;
    // ゲームパッドでジャンプ
    HandleGamepadJump(isOnFloor);
}

void Camera::HandleGamepadJump(bool isOnFloor)
{
    static WORD previousButtonState = 0; // 前フレームのボタン状態を保持する変数
    XINPUT_STATE joyState;
    if (Input::GetInstance()->GetJoystickState(joyState))
    {
        // 現在のボタン状態を取得
        WORD currentButtonState = joyState.Gamepad.wButtons;

        // ボタンの押された瞬間を検出
        bool buttonPressed = (currentButtonState & XINPUT_GAMEPAD_A && !(previousButtonState & XINPUT_GAMEPAD_A)) ||
            (currentButtonState & XINPUT_GAMEPAD_LEFT_SHOULDER && !(previousButtonState & XINPUT_GAMEPAD_LEFT_SHOULDER));

        // ボタンが押された瞬間のみジャンプを許可
        if (buttonPressed && isOnFloor && !isJumping)
        {
            // 地面にいるときのみジャンプを許可
            isJumping = true;
            jumpVelocity = JumpSpeed;
            Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioJumphandle_, false, 0.30f);
        }

#ifdef _DEBUG
        if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
        {
            // デバッグ用に地面の上昇
            transform_.translate.y += 1.0f;
            jumpVelocity = 0.0f;
            isJumping = false;
        }
        if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
        {
            // デバッグ用に地面の下降
            transform_.translate.y -= 1.0f;
            jumpVelocity = 0.0f;
            isJumping = false;
        }
#endif

        // 現在のボタン状態を次のフレームのために保持しておく
        previousButtonState = currentButtonState;
    }
}

float Camera::Face2Face(const Vector3& playerPosition, const Vector3 objectPosition) {
    // Y軸の回転角度を計算
    float deltaX = playerPosition.x - objectPosition.x;
    float deltaZ = playerPosition.z - objectPosition.z;
    float rotationY = atan2f(deltaX, deltaZ);
    return rotationY;
}

float Camera::Lerp(const float& a, const float& b, float t) {
    float result{};

    result = a + b * t;

    return result;
}
float Camera::Lerp2(float a, float b, float t) {
    return a + t * (b - a);
}

// 最短角度補間
float Camera::LerpShortAngle(float a, float b, float t)
{
    float diff = b - a;
    diff = std::fmod(diff + (float)std::numbers::pi, 2 * (float)std::numbers::pi) - (float)std::numbers::pi;
    return a + diff * t;
}

float Camera::LerpShortTranslate(float a, float b, float t) {
    return a + t * (b - a);
}

// 長さ(ノルム)
float Camera::Length(const Vector3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
};

void Camera::StagePreview(const Vector3& center, float radius, float speed, float angleX, bool& isPreview) {
    static float angle = 0.0f;
    static int lapCount = 0;
    static bool easingBack = false; // イージングで戻るフラグ
    static float easingProgress = 0.0f;
    static Vector3 easingStartPosition; // イージング開始時のカメラ位置
    Vector3 initialPosition = { 0.0f, 15.0f, -15.0f }; // 初期位置

    // コントローラー入力を取得
    XINPUT_STATE joyState;
    static bool buttonPressed = false;  // 前回のボタン状態
    static bool inputLocked = false;    // ボタン入力をロックするフラグ（イージング中）

    ZeroMemory(&joyState, sizeof(XINPUT_STATE));
    XInputGetState(0, &joyState);  // コントローラー1の状態を取得

    // ボタンの押下状態を確認
    bool currentButtonPress = joyState.Gamepad.wButtons != 0;

    // イージング中は新しいボタン入力を無視
    if (!inputLocked && currentButtonPress && !buttonPressed) {
        // ボタンが押された瞬間にイージング開始
        isEasing = true;
        easingBack = true;
        easingStartPosition = transform_.translate;  // イージング開始位置を保存
        inputLocked = true;  // ボタン入力をロック
    }

    // ボタンの状態を更新
    buttonPressed = currentButtonPress;

    if (isPreview) {
        if (!easingBack) {
            // 通常の円周移動
            angle += speed;

            // カメラを円周上に配置
            transform_.translate.x = center.x + radius * cosf(angle);
            transform_.translate.z = center.z + radius * sinf(angle);
            transform_.translate.y = center.y + 10.0f;  // 高さは固定

            // カメラの向きを中心に向ける
            transform_.rotate.y = Face2Face(center, transform_.translate);
            transform_.rotate.x = angleX;

            // 1周（2π）を超えた場合
            if (angle >= 2 * std::numbers::pi) {
                angle = 0.0f;  // 角度をリセット
                lapCount++;    // 周回数を増やす
            }

            // 1周と0.75周したらイージングで初期位置に戻る
            if (lapCount >= 1 && angle >= 1.5f * std::numbers::pi) {
                isEasing = true;
                easingBack = true;
                easingStartPosition = transform_.translate;  // イージング開始時の位置を保存
            }
        }
        else {
            // イージングで初期位置に戻る
            easingProgress += 0.005f;  // イージング進行度

            transform_.translate.x = Lerp2(easingStartPosition.x, initialPosition.x, easingProgress);
            transform_.translate.y = Lerp2(easingStartPosition.y, initialPosition.y, easingProgress);
            transform_.translate.z = Lerp2(easingStartPosition.z, initialPosition.z, easingProgress);

            // カメラの向きを中心に向ける（Lerp中でも中心を向かせる）
            transform_.rotate.y = Face2Face(center, transform_.translate);
            transform_.rotate.x = Lerp(angleX, -0.2f, easingProgress);

            // イージングが完了したら処理を終了
            if (easingProgress >= 1.0f) {
                lapCount = 0;        // 周回カウントをリセット
                angle = 0.0f;        // 角度をリセット
                easingBack = false;  // イージングフラグをリセット
                easingProgress = 0.0f;
                isPreview = false;   // プレビュー終了
                inputLocked = false; // ボタン入力のロックを解除
            }
        }

        // カメラの行列を更新
        cameraMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
        viewMatrix_ = Inverse(cameraMatrix_);
        projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, asepectRatio_, nearClip_, farClip_);
        viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
    }
}
