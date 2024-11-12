#include "WinAPI.h"
#include "GameManager.h"

/// <summary>
/// Windowsアプリケーションのエントリーポイント（WinMain関数）。
/// </summary>
/// <param name="hInstance">現在のインスタンスハンドル。</param>
/// <param name="hPrevInstance">前のインスタンスハンドル（使用されません）。</param>
/// <param name="lpCmdLine">コマンドライン引数（使用されません）。</param>
/// <param name="nCmdShow">ウィンドウの表示状態（使用されません）。</param>
/// <returns>アプリケーションの終了コード。</returns>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ゲームマネージャーのユニークポインタを作成
    std::unique_ptr<GameManager> gameManager = std::make_unique<GameManager>();
    // ゲームのメインループを実行
    gameManager->Run();
    // アプリケーションを正常終了
    return 0;
}
