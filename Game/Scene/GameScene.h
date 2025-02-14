#pragma once

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
#include "DirectXMath.h"
#include "Timer.h"
#include "Collider.h"
#include "Menu.h"
#include "random"
#include <vector>
#include <string>

class GameScene : public IScene
{
public:
    // Public methods
    void Init() override;
    void Update() override;
    void Draw() override;
    void PostDraw() override;
    void Release() override;
    int GameClose() override;
 
private:
    // Initialization methods
    void LoadTextures();
    void LoadModels();
    void LoadAudio();
    void InitializeData();
    void InitializeParticles();
    void InitStar();


    // Update methods
    void UpdatePortalCollision(const Vector3& playerPos);
    void UpdateEffects();
    void HandleSceneTransition();
    void UpdateStageTimes();
    void AlignObjectsToCamera();
    void HandleGamePadInput();
    void UpdatePlayerFloorCollision(const Vector3& playerPos);
    void UpdateLerpAnimations(const Vector3& playerPos);
    void UpdateObjects();
    void UpdateCamera();
    void HandleStartButton(const XINPUT_STATE& joyState);
    void HandleMenuNavigation(const XINPUT_STATE& joyState);
    void UpdateCameraFOV(const XINPUT_STATE& joyState);
    void DisplayDebugInfo(const Vector3& playerPos);
    void UpdateFloorInteraction();
    void ImguiDebug();
    void Remake();

    // Draw methods
    void DrawConeObjects();
    void DrawTitleTextObjects();
    void DrawTitleNumberObjects();
    void DrawSpecialObjects();
    void DrawParticles();

    // Member variables
    // World transforms
    WorldTransform worldTransformPa;
    WorldTransform worldTransformPa1;
    WorldTransform worldTransformPa2;
    WorldTransform worldTransformPa3;
    WorldTransform TitleTenQTransform;
    WorldTransform GameTenQTransform;

    // Scene state variables
    int sceneTime = 0;
    int sceneTime1 = 0;
    int selectedIndex1 = 0;
    int selectedIndex2 = 0;
    int selectedIndex3 = 0;
    int portal = 0;
    int indices[4] = { 0, 1, 3, 4 };
    float Textlerpindices[6] = { 25.0f, 12.5f, 7.5f, 7.5f, 7.5f, 7.5f };
    float textlerpindices[6] = { 20.0f, 7.5f, 6.5f, 6.5f, 6.5f, 6.5f };
    bool effect = false;
    bool effect2 = false;
    bool isOnFloor = false;
    bool isTitle = false;
    bool isDemo = false;  // Portal 1
    bool isGame = false;  // Portal 2
    bool isGame2 = false; // Portal 3
    bool isGame3 = false; // Portal 4
    bool isClear = false;
    bool isFadeInStarted = false;
    bool isMenu = false;
    bool startButtonPressed = false;
    bool isPreview = true;
    bool previousIsPreview = isPreview;
    // Camera and input
    Camera* camera = nullptr;
    Input* input = nullptr;

    // UI components
    Fade* fade = nullptr;
    Menu* menu = nullptr;

    // Timer and collision
    Timer timer;
    Collider* collider = nullptr;

    // Game objects
    Object3d* TenQOBJ = nullptr;
    Object3d* PositionOBJ = nullptr;
    std::vector<Object3d*> ConeObject_;
    std::vector<Object3d*> StarObject_;
    std::vector<Object3d*> TitleObject_;
    std::vector<Object3d*> TitleTextObject_;
    std::vector<Object3d*> TitleNumberObject_;

    // Particle system
    Engine::Particle* particleSystem_ = nullptr;
    Engine::Particle* particle = nullptr;
    Engine::Particle* particle1 = nullptr;
    Engine::Particle* particle2 = nullptr;
    Engine::Particle* particle3 = nullptr;
    Engine::Particle* firework = nullptr;
    Engine::Particle* firework2 = nullptr;
    Engine::Emitter emitter_;
    Engine::Emitter ParticleEmitter_;
    Engine::Emitter fireworkEmitter_;
    Engine::Emitter fireworkEmitter_2;
    Engine::RandRangePro randRange_;
    Engine::RandRangePro demoRandPro;
    Engine::RandRangePro fireworkRange_;
    Engine::RandRangePro fireworkRange_2;
    float rotateSize_ = 1.057f;
    //stagepreview
    Vector3 stageCenter[4] = {
    {0.0f, 30.0f, 40.0f},
    {0.0f, 70.0f, 100.0f},
    {0.0f, 20.0f, 40.0f},
    {0.0f, 80.0f, 100.0f}
    };
    // ステージの中心
    int nowStage = 0;
    float angleX[4] = { 0.2f,0.2f,0.2f,0.02f };
    float stageRadius[4] = { 150.0f,250.0f,130.0f,350.0f };                 // 円の半径
    float rotationSpeed[4] = { 0.02f,0.02f,0.02f,0.02f };               // カメラの回転速度
    // Post-processing
    PostProcess* postProcess_ = nullptr;

    // Resource handles
    uint32_t FADEtextureHandle;
    uint32_t FADE2textureHandle;
    uint32_t FADE3textureHandle;
    uint32_t MENUMEDItextureHandle;
    uint32_t MENUHIGHtextureHandle;
    uint32_t MENULOWtextureHandle;
    uint32_t WHITEtextureHandle;
    uint32_t PARTICLERED;
    uint32_t PARTICLESTAR;
    uint32_t PARTICLEBLUE;
    uint32_t PARTICLE3;
    uint32_t PARTICLE4;
    uint32_t BLUEtextureHandle;
    uint32_t CONEtextureHandle;
    uint32_t GRIDtextureHandle;
    uint32_t TITLETENQtextureHandle;
    uint32_t GAMETENQtextureHandle;
    uint32_t POSITIONtextureHandle;
    uint32_t STARtextureHandle;

    // Audio handles
    uint32_t AudioBGMhandle_;
    uint32_t AudioPortalhandle_;
    uint32_t AudioTimeCounthandle_;
    uint32_t AudioTimeCount2handle_;
    uint32_t explosionSound;

    // Debug variables
    Vector3 previousPos[99];
};
