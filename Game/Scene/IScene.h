#pragma once
//シーン名を列挙型(Enum)で定義
enum SCENE { TITLE, DEMO, Stage1, Stage2, Stage3 };

// シーン内での処理を行う基底クラス
class IScene {
protected:
	// シーン番号を管理する変数
	static int sceneNo;
	static bool TitleRoop;
	static bool DemoRoop;
	static bool GameRoop;
	static bool Game2Roop;
	static bool Game3Roop;
	static int DemoTime[5];
	static int SCENE1Time[5];
	static int SCENE2Time[5];
	static int SCENE3Time[5];
	static int SCENE4Time[5];
	static int menucount;
	static bool menuposition;
	// シーン番号を管理する変数
	int sceneNo = TITLE;
public:
	// 継承先で実装される関数
	/// 抽象クラスなので純粋仮想関数とする
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void PostDraw() = 0;
	virtual void Release() = 0;
	virtual int GameClose() = 0;

	// 仮想デストラクタを用意しないと警告される
	virtual ~IScene();

	// シーン番号のゲッターとセッター
	int GetSceneNo() const { return sceneNo; }
	void SetSceneNo(int newSceneNo) { sceneNo = newSceneNo; }


};
