#include "WorldTransform.h"
#include "mathFunction.h"

/// <summary>
/// ワールド行列を初期化する関数
/// </summary>
void WorldTransform::Initialize() {
    // スケール、回転、平行移動を合成してワールド行列を計算
    matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
    // 親が存在する場合は、親のワールド行列を掛け合わせる
    if (parent_) {
        matWorld_ = Multiply(matWorld_, parent_->matWorld_);
    }
}

/// <summary>
/// 定数バッファを作成する関数
/// </summary>
void WorldTransform::CreateConstBuffer() {
    // 定数バッファの作成処理を実装（未実装）
}

/// <summary>
/// 定数バッファへのマッピングを行う関数
/// </summary>
void WorldTransform::Map() {
    // 定数バッファのマッピング処理を実装（未実装）
}

/// <summary>
/// ワールド行列を定数バッファに転送する関数
/// </summary>
void WorldTransform::TransferMatrix() {
    // 行列データを定数バッファに転送する処理を実装（未実装）
}

/// <summary>
/// ワールド行列を更新する関数
/// </summary>
void WorldTransform::UpdateMatrix() {
    // スケール、回転、平行移動を合成してワールド行列を計算
    matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

    // 親が存在する場合は、親のワールド行列を掛け合わせる
    if (parent_) {
        matWorld_ = Multiply(matWorld_, parent_->matWorld_);
    }

    // 定数バッファにワールド行列を転送
    // TransferMatrix();
}
