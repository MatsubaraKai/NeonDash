#pragma once
#include"WinAPI.h"
#include "imgui.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "mathFunction.h"
#include <numbers>
#include "Input.h"
#include "Audio.h"
#include "IScene.h"

class Camera
{
public:
	void Initialize();
	void Update();
	void StagePreview(const Vector3& center, float radius, float speed,float angleX,bool& isPreview);
	void Move(int menucount);
	void HandleGamepadMovement(int menucount);
	void HandleRightStick(const XINPUT_STATE& joyState, int menucount);
	void HandleGamepadJump(bool isOnFloor);
	void Jump(bool isOnFloor);
	float Face2Face(const Vector3& playerPosition, const Vector3 objectPosition);
	float Lerp(const float& a, const float& b, float t);
	float Lerp2(float a, float b, float t);
	float LerpShortAngle(float a, float b, float t);
	float LerpShortTranslate(float a, float b, float t);
	float Length(const Vector3& v);

public: //Getter
	const Transform& GetTransform() const { return transform_; }
	const Vector3& GetTranslate() const { return transform_.translate; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetScale() const { return transform_.scale; }
	const Matrix4x4& GetCameraMatrix() const { return cameraMatrix_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewprojectionMatrix() const { return viewProjectionMatrix_; }

public: //Setter
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetFOV(float fovY);
	void SetAspectRario() {}
	void SetNearClip() {}
	void SetFarClip() {}
	void CameraDebug();
	Transform transform_;
	float fovY_ = 0.8f;
	float lookSensitivity;
	bool isEasing = false;
	bool isDie = false;
private:
	Matrix4x4 worldmatrix_;
	Matrix4x4 cameraMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	float asepectRatio_ = float(WinAPI::kClientWidth_) / float(WinAPI::kClientHeight_);
	float nearClip_ = 0.1f;
	float farClip_ = 10000.0f;
	//Move
	const float Gravity = -0.01f;
	const float GroundLevel = 3.0f;

	float PlayerSpeed = 0.3f;
	float angle_ = 0.0f;

	float JumpSpeed = 0.2f;
	bool isJumping = false;
	bool isFalling = false;
	float jumpVelocity = 0.0f;
	float previousJumpVelocity = 0.0f;
	uint32_t AudioJumphandle_ = Audio::SoundLoadWave("Resources/game/Audio/Jump.wav");
	uint32_t AudioPortalhandle_ = Audio::SoundLoadWave("Resources/game/Audio/portal.wav");

	int SE = 0;
};