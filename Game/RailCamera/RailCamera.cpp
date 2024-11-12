#include "RailCamera.h"

/// <summary>
/// レールカメラの初期化を行う関数
/// </summary>
void RailCamera::Init()
{
    // カメラオブジェクトを生成して初期化
    camera_ = std::make_unique<Camera>();
    camera_->Initialize();

    // カメラの回転を設定
    camera_->SetRotate({ 0.16f, 0.0f, 0.0f });
}

/// <summary>
/// レールカメラの更新処理を行う関数
/// </summary>
void RailCamera::Update()
{
    // カメラの更新処理を呼び出す
    camera_->Update();
}
