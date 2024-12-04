#pragma once
#include <iostream>

// シーン内での処理を行う基底クラス
class IScene {
protected:
    static int sceneNo;  // 現在のシーン番号

public:
    IScene() = default;

    // 仮想関数として定義することで、派生クラスが具体的に実装を提供する
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void PostDraw() = 0;
    virtual void Release() = 0;
    virtual int GameClose() = 0;

    // 仮想デストラクタを用意することで、ポインタ経由で破棄されるときに派生クラスのデストラクタも呼ばれる
    virtual ~IScene() = default;

    // シーン番号のゲッターとセッター
    static int GetSceneNo() { return sceneNo; }
    static void SetSceneNo(int newSceneNo) { sceneNo = newSceneNo; }
};

