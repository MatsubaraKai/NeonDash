#pragma once
#include <cmath>
/// <summary>
/// 3次元ベクトル
/// </summary>

struct Vector3 final {
    float x, y, z;

    // 正規化関数の追加
    Vector3 Normalize() const {
        float length = std::sqrt(x * x + y * y + z * z);
        if (length != 0) {
            return Vector3(x / length, y / length, z / length);
        }
        return *this; // 0ベクトルの場合はそのまま返す
    }
    // Dot関数の定義
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    // スカラー乗算のオーバーロード
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    // - 演算子のオーバーロード
    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }
    // + 演算子のオーバーロード
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
   
};