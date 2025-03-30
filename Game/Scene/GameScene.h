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
#include "ImGuiCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "mathFunction.h"
#include "PSOPostEffect.h"
#include "Loader.h"
#include "Audio.h"
#include <iostream>
#include <cmath>
struct ParticleWorldTransformData {
    WorldTransform* transform;
    Vector3 translation;
};

struct SceneData {
    std::string modelName;
    std::string coneFile;
    std::string starFile;
    std::string textFile;
    std::string fadeTexture;
};

extern std::map<std::string, SceneData> sceneTable;

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
    void InitItem();

    void TransitionScene(const std::string& sceneName);
    uint32_t GetTextureID(const std::string& textureName);
    // Update methods
    void UpdatePortalCollision(const Vector3& playerPos);
    void UpdateEffects();
    void HandleSceneTransition();
    void UpdateStageTimes();
    void AlignObjectsToCamera();
    void HandleGamePadInput();
    void UpdatePlayerFloorCollision(const Vector3& playerPos);
    void UpdateObjects();
    void UpdateCamera();
    void HandleStartButton(const XINPUT_STATE& joyState);
    void HandleMenuNavigation(const XINPUT_STATE& joyState);
    void UpdateCameraFOV(const XINPUT_STATE& joyState);
    void DisplayDebugInfo(const Vector3& playerPos);
    void UpdateFloorInteraction();
    void ImguiDebug();
    void Remake();
    void Resize();
    // 指定範囲内にプレイヤーがいるか判定
    bool IsPlayerInRange(const Vector3& pos, float minX, float maxX, float minZ, float maxZ);
    // Lerp適用
    void ApplyLerp(float& target, float goal, float speed);
    // タイトルテキストの高さ調整
    void MoveTextObjects(int sceneTime);
    // タイトルナンバーオブジェクトの高さ調整
    void MoveTitleNumberObjects(float targetHeight);
    // コーンオブジェクトの移動
    void MoveConeObjects(int sceneTime);
    // メイン処理
    void UpdateTitleScene(const Vector3& playerPos,int sceneTime);

    void StarSetting(const Vector3& playerPos);
    void ItemSetting(const Vector3& playerPos);

    void ClearMode();

    void StarCountNum();

    void MoveInCircle(Vector3& FloorPos, float deltaTime, float radius,
        const Vector3 centerPos, float& angle, float speed, int mode);

    // Draw methods
    void DrawObjects();
    void DrawTitleTextObjects();
    void DrawTitleNumberObjects();
    void DrawSpecialObjects();
    void DrawParticles();

    // Member variables
    // World transforms
    WorldTransform TitleSceneWorldTransformPa;
    WorldTransform DemoSceneWorldTransformPa;
    WorldTransform DemoSceneWorldTransformDemoPa;
    WorldTransform GameSceneWorldTransformPa;
    WorldTransform GameScene2WorldTransformPa;
    WorldTransform GameScene3WorldTransformPa;
    WorldTransform TitleTenQTransform;
    WorldTransform GameTenQTransform;

    //TitleMoveObj
    bool isTitle = false;
    float Textlerpindices[6] = { 25.0f, 12.5f, 7.5f, 7.5f, 7.5f, 7.5f };
    float textlerpindices[6] = { 20.0f, 7.5f, 6.5f, 6.5f, 6.5f, 6.5f };
    int indices[4] = { 0, 1, 3, 4 };//動かすText
    int TitleLerpFloor[2] = { 17,18 };//動かす床番号
    float LerpFloorSPos[2] = { 60.0f,55.0f };//LerpStartPos
    float LerpFloorEPos[2] = { -4.0f,-50.0f };//LerpEndPos

    //DemoScene
    bool FirstDemoFlag = false;
    bool isDemo = false;  // Portal 1

    //STAGE1
    bool isGame = false;  // Portal 2
    float Textlerpindices1[6] = { 8.00f,8.61f,4.5f,4.5f,0.5f,7.5f };
    float textlerpindices1[6] = { 6.00f,7.61f,3.5f,3.5f,-0.5f,6.5f };
    int indices1[6] = { 0, 1, 2, 3, 4, 6 };
    int GameScene1LerpFloor[2] = { 1,3 };
    float LerpFloorSPos1[2] = { 0.0f,-7.0f };
    float LerpFloorEPos1[2] = { 100.0f,55.0f };
    //STAGE2
    bool isGame2 = false; // Portal 3
    int Stage2indices[14] = { 2, 4, 6, 8,10,12,14,16,18,19,21,23,25,27 };
    int Stage2indices2[13] = { 1,3,5,7,9,11,13,15,17,20,22,24,26 };

    //STAGE3
    bool isGame3 = false; // Portal 4
    int Stage3indices[2] = { 2,3 };
    int Stage3indices2[4] = { 5,7,8,11 };
    int Stage3indices3[4] = { 6,9,10,12 };
    float Conelerpindices[2] = { -18.0f,18.0f };
    float conelerpindices[2] = { -100.0f,100.0f };

    //clear
    bool isStageClear = false; // clear
    // Scene state variables
    int sceneTime = 0;
    int sceneTime1 = 0;
    int selectedIndex[4] = { 0,0,0,0 };
    int portal = 0;
   
    bool effect = false;
    bool effect2 = false;
    bool isOnFloor = false;
    bool isClear = false;
    bool isFadeInStarted = false;
    bool isMenu = false;
    bool startButtonPressed = false;
    bool isPreview = true;
    bool previousIsPreview = isPreview;
    bool isGetStar = false;
    bool isGetItem = false;
    bool isStart = false;
    bool ignoreEffect = false; // 初期状態では無視しない
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
    Object3d* StarCountNumber = nullptr;
    std::vector<Object3d*> ConeObject_;
    std::vector<Object3d*> StarObject_;
    std::vector<Object3d*> ItemObject_;
    std::vector<Object3d*> TextObject_;
    std::vector<Object3d*> NumberObject_;

    Vector3 center = { 0.0f, 0.0f, -15.0f };
    Vector3 Itemcenter = { 0.0f, 3.2f, -15.0f };
    float angle = 0.0f;
    float Itemangle = 0.0f;
    float deltatime = 0.1f;
    float Itemdeltatime = -0.2f;
    // インデックスを定義
    enum ParticleType {
        FLOOR,
        CLEAR,
        TITLESCENE,
        DEMOSCENE,
        FAKEPARTICLE,
        STAGE1,
        STAGE2,
        STAGE3,
        FIREWORK1,
        FIREWORK2,
        PARTICLE_COUNT
    };

    // Particle system
    std::array<Engine::Particle*, PARTICLE_COUNT> particles = { nullptr };

    Engine::Emitter emitter_;
    Engine::Emitter ParticleEmitter_;
    Engine::Emitter fireworkEmitter_;
    Engine::Emitter fireworkEmitter_2;
    Engine::RandRangePro randRange_;
    Engine::RandRangePro demoRandPro;
    Engine::RandRangePro fireworkRange_;
    Engine::RandRangePro fireworkRange_2;
    //Titleのポータルの位置
    Vector3 PortalPosition[6] = {
        {-20.0f,1.5f,-17.5f},
        {-25.0f,1.5f,12.5f},
        {-2.5f, 1.5f, -32.35f},
        {25.0f, 1.5f, 12.5f},
        {25.0f, 1.5f, 0.0f},
        {25.0f, 1.5f, -12.5f},
    };
    //クリアポータルの位置
    Vector3 ClearPortalPosition[4] = {
        { -2.5f,2.5f,82.0f },
        { -2.5f,55.5f,220.0f },
        { -2.5f,7.5f,75.0f },
        { -2.5f,1.5f,122.0f }
    };
    //クリアポータルの上の数字
    Vector3 ClearNumberPosition[4] = {
    { 0.0f,8.5f,84.5f },
    { 0.0f,61.5f,222.5f },
    { 0.0f,13.5f,77.5f },
    { 0.0f,7.5f,124.5f }
    };
    //stagepreviewの時の中心点
    Vector3 stageCenter[5] = {
    {0.0f, 30.0f, 0.0f},
    {0.0f, 30.0f, 40.0f},
    {0.0f, 70.0f, 100.0f},
    {0.0f, 20.0f, 40.0f},
    {0.0f, 80.0f, 100.0f}
    };
    //ステージごとのスターの数
    int StarCount[4] = { 2,3,4,5 };
    // ステージの中心
    float rotateSize_ = 1.057f;
    int nowStage = 0;
    float angleX[5] = { 0.2f,0.2f,0.2f,0.2f,0.02f };
    float stageRadius[5] = { 120.0f,150.0f,250.0f,130.0f,350.0f };                 // 円の半径
    float rotationSpeed[5] = { 0.02f,0.02f,0.02f,0.02f,0.02f };               // カメラの回転速度
    // Post-processing
    PostProcess* postProcess_ = nullptr;

    enum TextureID {
        FADE,
        FADE2,
        FADE3,
        MENUMEDI,
        MENUHIGH,
        MENULOW,
        WHITE,
        PARTICLERED,
        PARTICLESTAR,
        PARTICLEBLUE,
        PARTICLE3,
        PARTICLE4,
        BLUE,
        CONE,
        GRID,
        TITLETENQ,
        GAMETENQ,
        POSITION,
        STAR,
        ITEM,
        TEXTURE_COUNT // テクスチャの総数
    };

    std::array<uint32_t, TEXTURE_COUNT> textureHandles;

    enum AudioID {
        BGM,
        PORTAL,
        TIMECOUNT,
        TIMECOUNT2,
        FIREWORK,
        GETSE,
        AUDIO_COUNT // オーディオの総数
    };

    std::array<uint32_t, AUDIO_COUNT> audioHandle;
    
    // Debug variables
    std::vector<Vector3> previousPos;
};
