#pragma once
#include "Camera.h"
#include <memory>

/// <summary>
/// @file RailCamera.h
/// @brief レールカメラのクラス宣言とその機能を提供します。
/// </summary>

/// <summary>
/// レールに沿って移動するカメラを管理するクラス。
/// </summary>
class RailCamera
{
public:
    /// <summary>
    /// レールカメラを初期化する関数。
    /// </summary>
    void Init();

    /// <summary>
    /// レールカメラの更新処理を行う関数。
    /// </summary>
    void Update();

public: // getter
    /// <summary>
    /// カメラオブジェクトを取得する関数。
    /// </summary>
    /// <returns>カメラオブジェクトへのポインタ。</returns>
    Camera* GetCamera() { return camera_.get(); }

private:
    /// <summary>
    /// カメラオブジェクトを保持するユニークポインタ。
    /// </summary>
    std::unique_ptr<Camera> camera_ = nullptr;

};
