#include "Timer.h"

void Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
    running = true;
}

void Timer::stop() {
    if (running) {
        endTime = std::chrono::high_resolution_clock::now();
        running = false;
    }
}
double Timer::elapsedMilliseconds() const {
    std::chrono::time_point<std::chrono::high_resolution_clock> endTimePoint;
    if (running) {
        endTimePoint = std::chrono::high_resolution_clock::now();
    }
    else {
        endTimePoint = endTime;
    }
    return std::chrono::duration<double, std::milli>(endTimePoint - startTime).count();
}

double Timer::elapsedSeconds() const {
    return elapsedMilliseconds() / 1000.0;
}

int Timer::elapsedTensOfMinutes() const {
    return static_cast<int>(elapsedSeconds() / 600) % 10;
}

int Timer::elapsedMinutesOnly() const {
    return static_cast<int>(elapsedSeconds() / 60) % 10;
}

int Timer::elapsedTensOfSeconds() const {
    return static_cast<int>(elapsedSeconds() / 10) % 6;
}

int Timer::elapsedSecondsOnly() const {
    return static_cast<int>(elapsedSeconds()) % 10;
}