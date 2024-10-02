﻿#pragma once
#include "IScene.h"
#include "Triangle.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Xinput.h"
#include "Particle.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Model.h"
#include "PostProcess.h"
#include "Fade.h"
#include "GameManager.h"
#include "Timer.h"
#include "Collider.h"
#include "Menu.h"


class STAGE3 : public IScene
{
public:
	void Init() override;
	void Update()override;
	void Draw()override;
	void PostDraw()override;
	void Release()override;
	int GameClose()override;

	WorldTransform worldTransformPa;
	WorldTransform worldTransformPa2;
	WorldTransform TenQTransform;
	Object3d* TenQOBJ = nullptr;
	Object3d* TextOBJ = nullptr;
private:
	int sceneTime = 0;
	int sceneTime1 = 0;
	int selectedIndex1 = 0;
	int selectedIndex2 = 0;
	Vector3 previousPos[99];

	Camera* camera = nullptr;
	Input* input = nullptr;
	Fade* fade = nullptr;
	Menu* menu = nullptr;
	Timer timer;
	Collider* collider = nullptr;


	std::vector<Object3d*> ConeObject_;
	std::vector<Object3d*> StarObject_;
	Object3d* Number = nullptr;
	Particle* particle = nullptr;
	Particle* particle2 = nullptr;

	PostProcess* postProcess_ = nullptr;
	//変数
	uint32_t FADEtextureHandle;
	uint32_t WHITEtextureHandle;
	uint32_t BLUEtextureHandle;
	uint32_t MENUMEDItextureHandle;
	uint32_t MENUHIGHtextureHandle;
	uint32_t MENULOWtextureHandle;
	uint32_t CONEtextureHandle;
	uint32_t TENQtextureHandle;
	uint32_t GRIDtextureHandle;
	uint32_t STARtextureHandle;
	uint32_t AudioStarGetSEhandle_;
	uint32_t AudioPortalhandle_;
	uint32_t AudioTimeCounthandle_;
	uint32_t AudioTimeCount2handle_;

	int indices[2] = { 2,3 };
	float Conelerpindices[2] = { -18.0f,18.0f };
	float conelerpindices[2] = { -100.0f,100.0f };


	Emitter ParticleEmitter_;
	RandRangePro demoRandPro;
	float rotateSize_ = 1.057f;
	bool effectFlag = true;
	bool effect = false;
	bool effect2 = false;
	bool isOnFloor = false;
	bool isGetStar = false;
	bool isClear = false;
	bool isTitle = false;
	bool isFadeInStarted = false;
	bool isMenu = false;
	bool startButtonPressed = false;
	int starCount = 5;
	int portal = 0;
};
