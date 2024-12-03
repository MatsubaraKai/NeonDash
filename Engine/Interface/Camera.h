#pragma once
#include "WinAPI.h"
#include "imgui.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "mathFunction.h"
#include <numbers>
#include "Input.h"
#include "Audio.h"
#include "IScene.h"
#include <thread>
/// <summary>
/// @file Camera.h
/// @brief カメラクラスの宣言
/// </summary>
class Camera
{
public:
    /// <summary>
    /// カメラを初期化する関数
    /// </summary>
    void Initialize();

    /// <summary>
    /// カメラの更新処理を行う関数
    /// </summary>
    void Update();

    /// <summary>
    /// ステージのプレビューを行う関数
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="radius">半径</param>
    /// <param name="speed">回転速度</param>
    /// <param name="angleX">X軸の角度</param>
    /// <param name="isPreview">プレビュー中かどうかのフラグ</param>
    void StagePreview(const Vector3& center, float radius, float speed, float angleX, bool& isPreview);

    /// <summary>
    /// カメラの移動処理を行う関数
    /// </summary>
    /// <param name="menucount">メニューのカウント値</param>
    void Move(int menucount);

    /// <summary>
    /// ゲームパッドによる移動処理を行う関数
    /// </summary>
    /// <param name="menucount">メニューのカウント値</param>
    void HandleGamepadMovement(int menucount);

    /// <summary>
    /// 右スティックによる視点操作を行う関数
    /// </summary>
    /// <param name="joyState">ゲームパッドの状態</param>
    /// <param name="menucount">メニューのカウント値</param>
    void HandleRightStick(const XINPUT_STATE& joyState, int menucount);

    /// <summary>
    /// ゲームパッドによるジャンプ処理を行う関数
    /// </summary>
    /// <param name="isOnFloor">地面に接地しているかどうかのフラグ</param>
    void HandleGamepadJump(bool isOnFloor);

    /// <summary>
    /// ジャンプ処理を行う関数
    /// </summary>
    /// <param name="isOnFloor">地面に接地しているかどうかのフラグ</param>
    void Jump(bool isOnFloor);

    /// <summary>
    /// 2つの座標間の角度を計算する関数
    /// </summary>
    /// <param name="playerPosition">プレイヤーの位置</param>
    /// <param name="objectPosition">オブジェクトの位置</param>
    /// <returns>Y軸の回転角度（ラジアン）</returns>
    float Face2Face(const Vector3& playerPosition, const Vector3 objectPosition);

    /// <summary>
    /// 線形補間を行う関数
    /// </summary>
    /// <param name="a">開始値</param>
    /// <param name="b">終了値</param>
    /// <param name="t">補間係数（0～1）</param>
    /// <returns>補間結果</returns>
    float Lerp(const float& a, const float& b, float t);

    /// <summary>
    /// 線形補間を行う関数（別バージョン）
    /// </summary>
    /// <param name="a">開始値</param>
    /// <param name="b">終了値</param>
    /// <param name="t">補間係数（0～1）</param>
    /// <returns>補間結果</returns>
    float Lerp2(float a, float b, float t);

    /// <summary>
    /// 球面線形補間（Slerp）を行う関数
    /// </summary>
    /// <param name="a">開始ベクトル</param>
    /// <param name="b">終了ベクトル</param>
    /// <param name="t">補間係数（0～1）</param>
    /// <returns>補間結果のベクトル</returns>
    Vector3 Slerp(const Vector3& a, const Vector3& b, float t);

    /// <summary>
    /// 最短角度で線形補間を行う関数
    /// </summary>
    /// <param name="a">開始角度</param>
    /// <param name="b">終了角度</param>
    /// <param name="t">補間係数（0～1）</param>
    /// <returns>補間結果の角度（ラジアン）</returns>
    float LerpShortAngle(float a, float b, float t);

    /// <summary>
    /// 位置の線形補間を行う関数
    /// </summary>
    /// <param name="a">開始位置</param>
    /// <param name="b">終了位置</param>
    /// <param name="t">補間係数（0～1）</param>
    /// <returns>補間結果の位置</returns>
    float LerpShortTranslate(float a, float b, float t);

    /// <summary>
    /// ベクトルの長さを計算する関数
    /// </summary>
    /// <param name="v">ベクトル</param>
    /// <returns>ベクトルの長さ</returns>
    float Length(const Vector3& v);

public: // Getter
    /// <summary>
    /// トランスフォームを取得する関数
    /// </summary>
    /// <returns>Transformオブジェクト</returns>
    const Transform& GetTransform() const { return transform_; }

    /// <summary>
    /// 位置を取得する関数
    /// </summary>
    /// <returns>位置ベクトル</returns>
    const Vector3& GetTranslate() const { return transform_.translate; }

    /// <summary>
    /// 回転を取得する関数
    /// </summary>
    /// <returns>回転ベクトル</returns>
    const Vector3& GetRotate() const { return transform_.rotate; }

    /// <summary>
    /// スケールを取得する関数
    /// </summary>
    /// <returns>スケールベクトル</returns>
    const Vector3& GetScale() const { return transform_.scale; }

    /// <summary>
    /// カメラ行列を取得する関数
    /// </summary>
    /// <returns>カメラ行列</returns>
    const Matrix4x4& GetCameraMatrix() const { return cameraMatrix_; }

    /// <summary>
    /// ビュー行列を取得する関数
    /// </summary>
    /// <returns>ビュー行列</returns>
    const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }

    /// <summary>
    /// 射影行列を取得する関数
    /// </summary>
    /// <returns>射影行列</returns>
    const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }

    /// <summary>
    /// ビュー射影行列を取得する関数
    /// </summary>
    /// <returns>ビュー射影行列</returns>
    const Matrix4x4& GetViewprojectionMatrix() const { return viewProjectionMatrix_; }

public: // Setter
    /// <summary>
    /// 位置を設定する関数
    /// </summary>
    /// <param name="translate">位置ベクトル</param>
    void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

    /// <summary>
    /// 回転を設定する関数
    /// </summary>
    /// <param name="rotate">回転ベクトル</param>
    void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

    /// <summary>
    /// スケールを設定する関数
    /// </summary>
    /// <param name="scale">スケールベクトル</param>
    void SetScale(const Vector3& scale) { transform_.scale = scale; }

    /// <summary>
    /// 視野角を設定する関数
    /// </summary>
    /// <param name="fovY">視野角（ラジアン）</param>
    void SetFOV(float fovY);

    /// <summary>
    /// アスペクト比を設定する関数
    /// </summary>
    void SetAspectRario() {}

    /// <summary>
    /// 近クリップ面を設定する関数
    /// </summary>
    void SetNearClip() {}

    /// <summary>
    /// 遠クリップ面を設定する関数
    /// </summary>
    void SetFarClip() {}

    /// <summary>
    /// カメラのデバッグ情報を表示する関数
    /// </summary>
    void CameraDebug();

    /// <summary>
    /// トランスフォーム
    /// </summary>
    Transform transform_;

    /// <summary>
    /// 視野角（ラジアン）
    /// </summary>
    float fovY_ = 0.8f;

    /// <summary>
    /// 視点移動の感度
    /// </summary>
    float lookSensitivity;

    /// <summary>
    /// イージング中かどうかのフラグ
    /// </summary>
    bool isEasing = false;

    /// <summary>
    /// 死亡状態かどうかのフラグ
    /// </summary>
    bool isDie = false;

private:
    /// <summary>
    /// ワールド行列
    /// </summary>
    Matrix4x4 worldmatrix_;

    /// <summary>
    /// カメラ行列
    /// </summary>
    Matrix4x4 cameraMatrix_;

    /// <summary>
    /// ビュー行列
    /// </summary>
    Matrix4x4 viewMatrix_;

    /// <summary>
    /// 射影行列
    /// </summary>
    Matrix4x4 projectionMatrix_;

    /// <summary>
    /// ビュー射影行列
    /// </summary>
    Matrix4x4 viewProjectionMatrix_;

    /// <summary>
    /// アスペクト比
    /// </summary>
    float asepectRatio_ = float(WinAPI::kClientWidth_) / float(WinAPI::kClientHeight_);

    /// <summary>
    /// 近クリップ面
    /// </summary>
    float nearClip_ = 0.1f;

    /// <summary>
    /// 遠クリップ面
    /// </summary>
    float farClip_ = 10000.0f;

    /// <summary>
    /// 重力
    /// </summary>
    const float Gravity = -0.01f;

    /// <summary>
    /// 地面の高さ
    /// </summary>
    const float GroundLevel = 3.0f;

    /// <summary>
    /// プレイヤーの移動速度
    /// </summary>
    float PlayerSpeed = 0.3f;

    /// <summary>
    /// 角度
    /// </summary>
    float angle_ = 0.0f;

    /// <summary>
    /// ジャンプ速度
    /// </summary>
    float JumpSpeed = 0.2f;

    /// <summary>
    /// ジャンプ中かどうかのフラグ
    /// </summary>
    bool isJumping = false;

    /// <summary>
    /// 落下中かどうかのフラグ
    /// </summary>
    bool isFalling = false;

    /// <summary>
    /// ジャンプの速度
    /// </summary>
    float jumpVelocity = 0.0f;

    /// <summary>
    /// 前回のジャンプ速度
    /// </summary>
    float previousJumpVelocity = 0.0f;

    /// <summary>
    /// ジャンプ音のハンドル
    /// </summary>
    uint32_t AudioJumphandle_ = Audio::SoundLoadWave("Resources/game/Audio/Jump.wav");

    /// <summary>
    /// ポータル音のハンドル
    /// </summary>
    uint32_t AudioPortalhandle_ = Audio::SoundLoadWave("Resources/game/Audio/portal.wav");

    /// <summary>
    /// サウンドエフェクトの識別子
    /// </summary>
    int SE = 0;
};
