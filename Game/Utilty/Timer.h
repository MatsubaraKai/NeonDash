#pragma once
#include <chrono>

/// <summary>
/// @file Timer.h
/// @brief 高精度な時間計測を行うタイマークラスの宣言。
/// </summary>

/// <summary>
/// 高精度クロックを使用して時間計測を行うタイマークラス。
/// </summary>
class Timer {
public:
    /// <summary>
    /// タイマーを開始する関数。
    /// </summary>
    void start();

    /// <summary>
    /// タイマーを停止する関数。
    /// </summary>
    void stop();

    /// <summary>
    /// 経過時間をミリ秒単位で取得する関数。
    /// </summary>
    /// <returns>経過時間（ミリ秒）。</returns>
    double elapsedMilliseconds() const;

    /// <summary>
    /// 経過時間を秒単位で取得する関数。
    /// </summary>
    /// <returns>経過時間（秒）。</returns>
    double elapsedSeconds() const;

    /// <summary>
    /// 経過時間の10の位の分を取得する関数。
    /// </summary>
    /// <returns>経過時間の10の位の分（0〜9）。</returns>
    int elapsedTensOfMinutes() const;

    /// <summary>
    /// 経過時間の1の位の分を取得する関数。
    /// </summary>
    /// <returns>経過時間の1の位の分（0〜9）。</returns>
    int elapsedMinutesOnly() const;

    /// <summary>
    /// 経過時間の10の位の秒を取得する関数。
    /// </summary>
    /// <returns>経過時間の10の位の秒（0〜5）。</returns>
    int elapsedTensOfSeconds() const;

    /// <summary>
    /// 経過時間の1の位の秒を取得する関数。
    /// </summary>
    /// <returns>経過時間の1の位の秒（0〜9）。</returns>
    int elapsedSecondsOnly() const;

private:
    /// <summary>
    /// タイマーの開始時刻。
    /// </summary>
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    /// <summary>
    /// タイマーの終了時刻。
    /// </summary>
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

    /// <summary>
    /// タイマーが動作中かどうかを示すフラグ。
    /// </summary>
    bool running;
};
