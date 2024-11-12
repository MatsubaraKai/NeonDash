#include "TimedCall.h"

/// <summary>
/// コンストラクタ。指定された時間後に関数を呼び出すタイマを初期化します。
/// </summary>
/// <param name="p">タイマー終了時に実行する関数オブジェクト。</param>
/// <param name="time">タイマーのカウントダウン時間（フレーム数などの単位）。</param>
TimedCall::TimedCall(std::function<void()> p, uint32_t time)
{
    // カウントダウン時間を設定
    time_ = time;
    // コールバック関数を設定
    pFunc_ = p;
}

/// <summary>
/// タイマーの更新処理を行う関数。時間が経過すると指定された関数を呼び出します。
/// </summary>
void TimedCall::Update()
{
    // タイマーが既に終了している場合は何もしない
    if (isFinish_) {
        return;
    }
    // カウントダウンをデクリメント
    time_--;
    // 時間がゼロ以下になった場合
    if (time_ <= 0) {
        // タイマーを終了状態に設定
        isFinish_ = true;
        // コールバック関数を実行
        pFunc_();
    }
}
