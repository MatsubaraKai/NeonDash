#include "Timer.h"

/// <summary>
/// タイマーを開始する関数
/// </summary>
void Timer::start() {
    // 現在の高精度クロックの時間を取得し、開始時間に設定
    startTime = std::chrono::high_resolution_clock::now();
    // タイマーが動作中であることを示すフラグを設定
    running = true;
}

/// <summary>
/// タイマーを停止する関数
/// </summary>
void Timer::stop() {
    // タイマーが動作中の場合のみ処理を行う
    if (running) {
        // 現在の高精度クロックの時間を取得し、終了時間に設定
        endTime = std::chrono::high_resolution_clock::now();
        // タイマーが停止していることを示すフラグを設定
        running = false;
    }
}

/// <summary>
/// 経過時間をミリ秒単位で取得する関数
/// </summary>
/// <returns>経過時間（ミリ秒）</returns>
double Timer::elapsedMilliseconds() const {
    // 終了時間を格納する変数を宣言
    std::chrono::time_point<std::chrono::high_resolution_clock> endTimePoint;
    // タイマーが動作中かどうかで終了時間を設定
    if (running) {
        // 現在の時間を終了時間とする
        endTimePoint = std::chrono::high_resolution_clock::now();
    }
    else {
        // 停止時の終了時間を使用
        endTimePoint = endTime;
    }
    // 経過時間を計算し、ミリ秒単位で返す
    return std::chrono::duration<double, std::milli>(endTimePoint - startTime).count();
}

/// <summary>
/// 経過時間を秒単位で取得する関数
/// </summary>
/// <returns>経過時間（秒）</returns>
double Timer::elapsedSeconds() const {
    // ミリ秒単位の経過時間を取得し、1000で割って秒単位に変換
    return elapsedMilliseconds() / 1000.0;
}

/// <summary>
/// 経過時間の10の位の分を取得する関数
/// </summary>
/// <returns>経過時間の10の位の分（0〜9）</returns>
int Timer::elapsedTensOfMinutes() const {
    // 経過時間を秒単位で取得し、600で割って10の位の分を計算
    return static_cast<int>(elapsedSeconds() / 600) % 10;
}

/// <summary>
/// 経過時間の1の位の分を取得する関数
/// </summary>
/// <returns>経過時間の1の位の分（0〜9）</returns>
int Timer::elapsedMinutesOnly() const {
    // 経過時間を秒単位で取得し、60で割って1の位の分を計算
    return static_cast<int>(elapsedSeconds() / 60) % 10;
}

/// <summary>
/// 経過時間の10の位の秒を取得する関数
/// </summary>
/// <returns>経過時間の10の位の秒（0〜5）</returns>
int Timer::elapsedTensOfSeconds() const {
    // 経過時間を秒単位で取得し、10で割って10の位の秒を計算
    return static_cast<int>(elapsedSeconds() / 10) % 6;
}

/// <summary>
/// 経過時間の1の位の秒を取得する関数
/// </summary>
/// <returns>経過時間の1の位の秒（0〜9）</returns>
int Timer::elapsedSecondsOnly() const {
    // 経過時間を秒単位で取得し、1の位の秒を計算
    return static_cast<int>(elapsedSeconds()) % 10;
}
