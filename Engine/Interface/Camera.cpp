#include "Camera.h"
#include <algorithm>

/// <summary>
/// カメラの初期化を行う関数
/// </summary>
void Camera::Initialize() {
    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-80.0f} };
    // カメラ行列の計算
    cameraMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(cameraMatrix_);
    projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, asepectRatio_, nearClip_, farClip_);
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

/// <summary>
/// カメラの更新処理を行う関数
/// </summary>
void Camera::Update() {
    // カメラ行列の再計算
    cameraMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(cameraMatrix_);
    projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, asepectRatio_, nearClip_, farClip_);
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
    if (transform_.rotate.y >= 6.283f || transform_.rotate.y <= -6.283f) {
        transform_.rotate.y = 0.0f;
    }
}

/// <summary>
/// カメラのデバッグ情報を表示する関数
/// </summary>
void Camera::CameraDebug()
{
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.0f, 0.7f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.1f, 0.0f, 0.3f, 0.5f));
    ImGui::Begin("camera");

    // FOVの調整
    ImGui::DragFloat("FOV", &fovY_, 0.01f);

    // 位置・回転・スケールの調整
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

/// <summary>
/// カメラの移動処理を行う関数
/// </summary>
/// <param name="menucount">メニューのカウント値</param>
void Camera::Move(int menucount)
{
    // ゲームパッドによる移動処理
    HandleGamepadMovement(menucount);
}

/// <summary>
/// ゲームパッドでの移動処理を行う関数
/// </summary>
/// <param name="menucount">メニューのカウント値</param>
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

            // カメラの向きに合わせて移動方向を調整
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

            // 右スティックによる視点移動
            HandleRightStick(joyState, menucount);
        }
    }
}

/// <summary>
/// カメラの視野角を設定する関数
/// </summary>
/// <param name="fovY">視野角（ラジアン）</param>
void Camera::SetFOV(float fovY) {
    fovY_ = fovY;
}

/// <summary>
/// 右スティックでの視点移動を処理する関数
/// </summary>
/// <param name="joyState">ゲームパッドの状態</param>
/// <param name="menucount">メニューのカウント値</param>
void Camera::HandleRightStick(const XINPUT_STATE& joyState, int menucount)
{
    const float rightStickDeadZone = 0.1f;
    if (std::abs(joyState.Gamepad.sThumbRX) > rightStickDeadZone * SHRT_MAX ||
        std::abs(joyState.Gamepad.sThumbRY) > rightStickDeadZone * SHRT_MAX)
    {
        float rightStickX = (float)joyState.Gamepad.sThumbRX / SHRT_MAX * lookSensitivity;
        float rightStickY = (float)joyState.Gamepad.sThumbRY / SHRT_MAX * lookSensitivity;

        transform_.rotate.y += rightStickX;  // Yaw（左右の回転）
        transform_.rotate.x -= rightStickY;  // Pitch（上下の回転）

        // ピッチ角度の制限
        const float maxPitch = 45.0f * (float)std::numbers::pi / 180.0f;
        const float minPitch = -45.0f * (float)std::numbers::pi / 180.0f;
        transform_.rotate.x = std::clamp(transform_.rotate.x, minPitch, maxPitch);
    }
    // 感度の調整
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

/// <summary>
/// カメラのジャンプ処理を行う関数
/// </summary>
/// <param name="isOnFloor">地面に接しているかどうか</param>
void Camera::Jump(bool isOnFloor)
{
    static float easingProgress = 0.0f;
    static Vector3 initpos = { 0.0f,7.0f,-15.0f };

    if (isDie) {
        if (easingProgress == 0.0f) {
            Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioPortalhandle_, false, 0.30f);
        }
        jumpVelocity = 0.0f;
        isJumping = false;
        easingProgress += 0.0001f;  // イージングの進行度

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
        // 空中にいる場合の処理
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
        if (jumpVelocity == previousJumpVelocity) {
            jumpVelocity = 0.0f;
        }
        isJumping = false;
    }
    // 前フレームのジャンプ速度を保存
    previousJumpVelocity = jumpVelocity;
    // ゲームパッドでのジャンプ処理
    HandleGamepadJump(isOnFloor);
}

/// <summary>
/// ゲームパッドでのジャンプ処理を行う関数
/// </summary>
/// <param name="isOnFloor">地面に接しているかどうか</param>
void Camera::HandleGamepadJump(bool isOnFloor)
{
    static WORD previousButtonState = 0; // 前フレームのボタン状態
    XINPUT_STATE joyState;
    if (Input::GetInstance()->GetJoystickState(joyState))
    {
        // 現在のボタン状態を取得
        WORD currentButtonState = joyState.Gamepad.wButtons;

        // ボタンの押下を検出
        bool buttonPressed = (currentButtonState & XINPUT_GAMEPAD_A && !(previousButtonState & XINPUT_GAMEPAD_A)) ||
            (currentButtonState & XINPUT_GAMEPAD_LEFT_SHOULDER && !(previousButtonState & XINPUT_GAMEPAD_LEFT_SHOULDER));

        // ジャンプの実行
        if (buttonPressed && isOnFloor && !isJumping)
        {
            isJumping = true;
            jumpVelocity = JumpSpeed;
            Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), AudioJumphandle_, false, 0.30f);
        }

#ifdef _DEBUG
        if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
        {
            // デバッグ用：地面を上昇
            transform_.translate.y += 1.0f;
            jumpVelocity = 0.0f;
            isJumping = false;
        }
        if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
        {
            // デバッグ用：地面を下降
            transform_.translate.y -= 1.0f;
            jumpVelocity = 0.0f;
            isJumping = false;
        }
#endif

        // ボタン状態の更新
        previousButtonState = currentButtonState;
    }
}

/// <summary>
/// 2つの位置間のY軸回転角度を計算する関数
/// </summary>
/// <param name="playerPosition">プレイヤーの位置</param>
/// <param name="objectPosition">対象の位置</param>
/// <returns>Y軸の回転角度（ラジアン）</returns>
float Camera::Face2Face(const Vector3& playerPosition, const Vector3 objectPosition) {
    // Y軸の回転角度を計算
    float deltaX = playerPosition.x - objectPosition.x;
    float deltaZ = playerPosition.z - objectPosition.z;
    float rotationY = atan2f(deltaX, deltaZ);
    return rotationY;
}

/// <summary>
/// 線形補間を行う関数
/// </summary>
/// <param name="a">開始値</param>
/// <param name="b">終了値</param>
/// <param name="t">補間係数（0～1）</param>
/// <returns>補間された値</returns>
float Camera::Lerp(const float& a, const float& b, float t) {
    float result{};
    result = a + b * t;
    return result;
}

/// <summary>
/// 線形補間を行う関数（別バージョン）
    /// </summary>
/// <param name="a">開始値</param>
/// <param name="b">終了値</param>
/// <param name="t">補間係数（0～1）</param>
/// <returns>補間された値</returns>
float Camera::Lerp2(float a, float b, float t) {
    return a + t * (b - a);
}

/// <summary>
/// 球面線形補間（Slerp）を行う関数
/// </summary>
/// <param name="a">開始ベクトル</param>
/// <param name="b">終了ベクトル</param>
/// <param name="t">補間係数（0～1）</param>
/// <returns>補間されたベクトル</returns>
Vector3 Camera::Slerp(const Vector3& a, const Vector3& b, float t) {
    // ベクトル間の角度のコサインを計算
    float dot = a.Dot(b);

    // コサイン値をクランプ
    dot = std::fmax(-1.0f, std::fmin(1.0f, dot));

    // ベクトル間の角度を計算
    float theta = std::acos(dot) * t;

    // 相対ベクトルを正規化
    Vector3 relativeVec = (b - a * dot).Normalize();

    // 補間結果を返す
    return (a * std::cos(theta)) + (relativeVec * std::sin(theta));
}

/// <summary>
/// 最短角度で線形補間を行う関数
/// </summary>
/// <param name="a">開始角度</param>
/// <param name="b">終了角度</param>
/// <param name="t">補間係数（0～1）</param>
/// <returns>補間された角度</returns>
float Camera::LerpShortAngle(float a, float b, float t)
{
    float diff = b - a;
    diff = std::fmod(diff + (float)std::numbers::pi, 2 * (float)std::numbers::pi) - (float)std::numbers::pi;
    return a + diff * t;
}

/// <summary>
/// 位置の線形補間を行う関数
/// </summary>
/// <param name="a">開始位置</param>
/// <param name="b">終了位置</param>
/// <param name="t">補間係数（0～1）</param>
/// <returns>補間された位置</returns>
float Camera::LerpShortTranslate(float a, float b, float t) {
    return a + t * (b - a);
}

/// <summary>
/// ベクトルの長さを計算する関数
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトルの長さ</returns>
float Camera::Length(const Vector3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
};

/// <summary>
/// ステージプレビューを行う関数
/// </summary>
/// <param name="center">中心位置</param>
/// <param name="radius">半径</param>
/// <param name="speed">回転速度</param>
/// <param name="angleX">X軸の角度</param>
/// <param name="isPreview">プレビュー状態のフラグ</param>
void Camera::StagePreview(const Vector3& center, float radius, float speed, float angleX, bool& isPreview) {
    static float angle = 0.0f;
    static int lapCount = 0;
    static bool easingBack = false; // イージングで戻るフラグ
    static float easingProgress = 0.0f;
    static Vector3 easingStartPosition; // イージング開始位置
    Vector3 initialPosition = { 0.0f, 15.0f, -15.0f }; // 初期位置

    // コントローラー入力を取得
    XINPUT_STATE joyState;
    static bool buttonPressed = false;  // 前回のボタン状態
    static bool inputLocked = false;    // ボタン入力のロックフラグ

    ZeroMemory(&joyState, sizeof(XINPUT_STATE));
    XInputGetState(0, &joyState);  // コントローラー1の状態を取得

    // ボタンの押下状態を確認
    bool currentButtonPress = joyState.Gamepad.wButtons != 0;

    // イージング中はボタン入力を無視
    if (!inputLocked && currentButtonPress && !buttonPressed) {
        // イージング開始
        isEasing = true;
        easingBack = true;
        easingStartPosition = transform_.translate;
        inputLocked = true;
    }

    // ボタン状態の更新
    buttonPressed = currentButtonPress;

    if (isPreview) {
        if (!easingBack) {
            // 円周移動
            angle += speed;

            // カメラ位置の更新
            transform_.translate.x = center.x + radius * cosf(angle);
            transform_.translate.z = center.z + radius * sinf(angle);
            transform_.translate.y = center.y + 10.0f;

            // カメラの向きを中心に向ける
            transform_.rotate.y = Face2Face(center, transform_.translate);
            transform_.rotate.x = angleX;

            // 周回数のチェック
            if (angle >= 2 * std::numbers::pi) {
                angle = 0.0f;
                lapCount++;
            }

            // 一定周回後にイージング開始
            if (lapCount >= 1 && angle >= 1.5f * std::numbers::pi) {
                isEasing = true;
                easingBack = true;
                easingStartPosition = transform_.translate;
            }
        }
        else {
            // イージングで初期位置に戻る
            easingProgress += 0.005f;

            transform_.translate.x = Lerp2(easingStartPosition.x, initialPosition.x, easingProgress);
            transform_.translate.y = Lerp2(easingStartPosition.y, initialPosition.y, easingProgress);
            transform_.translate.z = Lerp2(easingStartPosition.z, initialPosition.z, easingProgress);

            // カメラの向きを中心に向ける
            transform_.rotate.y = Face2Face(center, transform_.translate);
            transform_.rotate.x = Lerp(angleX, -0.2f, easingProgress);

            // イージング完了時の処理
            if (easingProgress >= 1.0f) {
                lapCount = 0;
                angle = 0.0f;
                easingBack = false;
                easingProgress = 0.0f;
                isPreview = false;
                inputLocked = false;
            }
        }

        // カメラ行列の更新
        cameraMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
        viewMatrix_ = Inverse(cameraMatrix_);
        projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, asepectRatio_, nearClip_, farClip_);
        viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
    }
}
