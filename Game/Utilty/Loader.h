#pragma once
#include <string>
#include <vector>
#include "Transform.h"
#include "Model.h"
#include "Object3d.h"
#include "Camera.h"

/// <summary>
/// @file Loder.h
/// @brief レベルデータの読み込みとオブジェクト生成を行うクラスの宣言。
/// </summary>

/// <summary>
/// レベルデータを格納するクラス。
/// </summary>
class LevelData {
public:
    /// <summary>
    /// オブジェクトデータを表す構造体。
    /// </summary>
    struct ObjectData {
        /// <summary>
        /// モデルのファイル名。
        /// </summary>
        std::string filename;
        /// <summary>
        /// オブジェクトの変換情報（位置、回転、スケール）。
        /// </summary>
        Transform transform;
    };
    /// <summary>
    /// レベル内の全オブジェクトデータを格納するベクター。
    /// </summary>
    std::vector<ObjectData> objects;

private:
    // 将来的にプライベートメンバを追加可能。
};

/// <summary>
/// JSONファイルを読み込み、オブジェクトやカメラを生成するローダークラス。
/// </summary>
class Loader
{
public:
    /// <summary>
    /// JSONファイルを読み込み、オブジェクトとカメラを初期化する関数。
    /// </summary>
    /// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ。</param>
    /// <param name="fileName">読み込むJSONファイル名（拡張子なし）。</param>
    /// <param name="objects">生成したオブジェクトを格納するベクター。</param>
    /// <param name="camera">カメラオブジェクトへのポインタ。</param>
    static void LoadJsonFile(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects, Camera* camera);

    /// <summary>
    /// JSONファイルを読み込み、オブジェクトを初期化する関数（カメラ設定なし）。
    /// </summary>
    /// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ。</param>
    /// <param name="fileName">読み込むJSONファイル名（拡張子なし）。</param>
    /// <param name="objects">生成したオブジェクトを格納するベクター。</param>
    static void LoadJsonFile2(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects);

    static void LoadAllConeJsonFile(const std::string kDefaultBaseDirectory, const std::string fileName, const std::string sceneType, std::vector<Object3d*>& objects, Camera* camera);
    static void LoadAllStarJsonFile(const std::string kDefaultBaseDirectory, const std::string fileName, const std::string sceneType, std::vector<Object3d*>& objects);
    
        /// <summary>
    /// JSONファイルを読み込み、番号付きのオブジェクトを初期化する関数。
    /// </summary>
    /// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ。</param>
    /// <param name="fileName">読み込むJSONファイル名（拡張子なし）。</param>
    /// <param name="objects">生成したオブジェクトを格納するベクター。</param>
    static void LoadJsonFileNumber(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects);

    /// <summary>
    /// JSONファイルを読み込み、テキストオブジェクトを初期化する関数。
    /// </summary>
    /// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ。</param>
    /// <param name="fileName">読み込むJSONファイル名（拡張子なし）。</param>
    /// <param name="objects">生成したオブジェクトを格納するベクター。</param>
    static void LoadJsonFileText(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects);
    static float Lerp(const float& a, const float& b, float t);
};
