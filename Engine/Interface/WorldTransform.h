#pragma once

#include "Matrix4x4.h"
#include "Vector3.h"
#include "d3D12.h"
#include "wrl.h"

/// <summary>
/// 定数バッファ用データ構造体
/// </summary>
struct ConstBufferDataWorldTransform {
    /// <summary>
    /// ローカルからワールドへの変換行列
    /// </summary>
    Matrix4x4 matWorld; // ローカル → ワールド変換行列
};

/// <summary>
/// ワールド変換データを管理する構造体
/// </summary>
struct WorldTransform {
    /// <summary>
    /// 定数バッファ
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
    /// <summary>
    /// マッピング済みアドレス
    /// </summary>
    ConstBufferDataWorldTransform* constMap_ = nullptr;
    /// <summary>
    /// ローカルスケール
    /// </summary>
    Vector3 scale_ = { 1, 1, 1 };
    /// <summary>
    /// X, Y, Z軸回りのローカル回転角
    /// </summary>
    Vector3 rotation_{ 0, 0, 0 };
    /// <summary>
    /// ローカル座標
    /// </summary>
    Vector3 translation_ = { 0, 0, 0 };
    /// <summary>
    /// ローカルからワールドへの変換行列
    /// </summary>
    Matrix4x4 matWorld_;
    /// <summary>
    /// 親となるワールド変換へのポインタ
    /// </summary>
    const WorldTransform* parent_ = nullptr;

    /// <summary>
    /// ワールド変換の初期化を行う関数
    /// </summary>
    void Initialize();
    /// <summary>
    /// 定数バッファを作成する関数
    /// </summary>
    void CreateConstBuffer();
    /// <summary>
    /// 定数バッファへのマッピングを行う関数
    /// </summary>
    void Map();
    /// <summary>
    /// ワールド行列を定数バッファに転送する関数
    /// </summary>
    void TransferMatrix();
    /// <summary>
    /// ワールド行列を計算・更新する関数
    /// </summary>
    void UpdateMatrix();
};
