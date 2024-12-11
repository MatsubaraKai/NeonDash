#pragma once
#include <memory>
#include "IScene.h"
#include "Input.h"
#include "GameScene.h"
//#include "Input.h"
class GameManager
{
private:
	// どのステージを呼び出すかを管理する変数
	int currentSceneNo_ = 0;
	int prevSceneNo_ = 0;

public:
	const char* currentStatus = "Outline Blue";
	~GameManager();

	int Run();
};