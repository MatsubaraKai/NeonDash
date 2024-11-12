#pragma once

#include <functional>

/// <summary>
/// 時限発動を行うクラス。
/// </summary>
class TimedCall
{
public:
    /// <summary>
    /// コンストラクタ。指定した時間後に関数を呼び出すように初期化します。
    /// </summary>
    /// <param name="p">時間経過後に実行する関数オブジェクト。</param>
    /// <param name="time">発動までの時間（フレーム数などの単位）。</param>
    TimedCall(std::function<void()> p, uint32_t time);

    /// <summary>
    /// タイマーの更新処理を行う関数。
    /// </summary>
    void Update();

    /// <summary>
    /// タイマーが終了したかどうかを取得する関数。
    /// </summary>
    /// <returns>終了した場合はtrue、まだの場合はfalseを返す。</returns>
    bool IsFinished() {
        return isFinish_;
    }

private:
    /// <summary>
    /// 残り時間。
    /// </summary>
    uint32_t time_;

    /// <summary>
    /// タイマーが終了したかどうかのフラグ。
    /// </summary>
    bool isFinish_ = false;

    /// <summary>
    /// 時間経過後に実行する関数オブジェクト。
    /// </summary>
    std::function<void()> pFunc_;
};
