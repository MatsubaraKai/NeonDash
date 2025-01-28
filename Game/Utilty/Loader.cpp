#include "Loader.h"
#include "json.hpp"
#include <fstream>
#include <cassert>
#include "ModelManager.h"

/// <summary>
/// JSONファイルを読み込み、オブジェクトとカメラを初期化する関数
/// </summary>
/// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ</param>
/// <param name="fileName">読み込むJSONファイル名（拡張子なし）</param>
/// <param name="objects">生成したオブジェクトを格納するベクター</param>
/// <param name="camera">カメラオブジェクトへのポインタ</param>
void Loader::LoadJsonFile(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects, Camera* camera)
{
    // フルパスを生成
    const std::string fullpath = kDefaultBaseDirectory + "/" + fileName + ".json";

    // ファイルストリームを作成
    std::ifstream file;

    // ファイルを開く
    file.open(fullpath);
    // ファイルオープン失敗をチェック
    if (file.fail()) {
        assert(0);
    }

    // JSONデータを格納する変数
    nlohmann::json deserialized;

    // ファイルからJSONデータを読み込む
    file >> deserialized;

    // 正しいレベルデータファイルかチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    // "name"フィールドを取得
    std::string name = deserialized["name"].get<std::string>();
    // "scene"であることを確認
    assert(name.compare("scene") == 0);

    // レベルデータを格納するインスタンスを生成
    LevelData* levelData = new LevelData();

    // "objects"配列内の全オブジェクトを走査
    for (nlohmann::json& object : deserialized["objects"]) {
        assert(object.contains("type"));

        // オブジェクトの種別を取得
        std::string type = object["type"].get<std::string>();

        // MESHオブジェクトの場合
        if (type.compare("MESH") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            if (object.contains("file_name")) {
                // ファイル名を取得
                objectData.filename = object["file_name"];
            }

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -(float)transform["rotation"][0];
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];

            // モデルをロード
            ModelManager::GetInstance()->LoadModel("Resources/game/", objectData.filename + ".obj");
        }

        // 子オブジェクトが存在する場合（未実装）
        if (object.contains("children")) {
            // TODO: 再帰的に子オブジェクトを処理する
        }

        // CAMERAオブジェクトの場合
        if (type.compare("CAMERA") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -((float)transform["rotation"][0] - 3.141592f / 2.0f);
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];

            // カメラの位置と回転を設定
            camera->SetTranslate(objectData.transform.translate);
            camera->SetRotate(objectData.transform.rotate);
        }
    }

    // オブジェクトを生成し、配置する
    for (auto& objectData : levelData->objects) {
        // 3Dオブジェクトを生成
        Object3d* newObject3d = new Object3d();
        newObject3d->Init();
        newObject3d->SetModel(objectData.filename + ".obj");
        newObject3d->SetTransform(objectData.transform);
        // 配列に追加
        objects.push_back(newObject3d);
    }
}

/// <summary>
/// JSONファイルを読み込み、オブジェクトを初期化する関数（カメラ設定なし）
/// </summary>
/// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ</param>
/// <param name="fileName">読み込むJSONファイル名（拡張子なし）</param>
/// <param name="objects">生成したオブジェクトを格納するベクター</param>
void Loader::LoadJsonFile2(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects)
{
    // フルパスを生成
    const std::string fullpath = kDefaultBaseDirectory + "/" + fileName + ".json";

    // ファイルストリームを作成
    std::ifstream file;

    // ファイルを開く
    file.open(fullpath);
    // ファイルオープン失敗をチェック
    if (file.fail()) {
        assert(0);
    }

    // JSONデータを格納する変数
    nlohmann::json deserialized;

    // ファイルからJSONデータを読み込む
    file >> deserialized;

    // 正しいレベルデータファイルかチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    // "name"フィールドを取得
    std::string name = deserialized["name"].get<std::string>();
    // "scene"であることを確認
    assert(name.compare("scene") == 0);

    // レベルデータを格納するインスタンスを生成
    LevelData* levelData = new LevelData();

    // "objects"配列内の全オブジェクトを走査
    for (nlohmann::json& object : deserialized["objects"]) {
        assert(object.contains("type"));

        // オブジェクトの種別を取得
        std::string type = object["type"].get<std::string>();

        // MESHオブジェクトの場合
        if (type.compare("MESH") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            if (object.contains("file_name")) {
                // ファイル名を取得
                objectData.filename = object["file_name"];
            }

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -(float)transform["rotation"][0];
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];

            // モデルをロード
            ModelManager::GetInstance()->LoadModel("Resources/game/", objectData.filename + ".obj");
        }

        // 子オブジェクトが存在する場合（未実装）
        if (object.contains("children")) {
            // TODO: 再帰的に子オブジェクトを処理する
        }

        // CAMERAオブジェクトの場合（カメラ設定なし）
        if (type.compare("CAMERA") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -((float)transform["rotation"][0] - 3.141592f / 2.0f);
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];

            // カメラ設定は行わない
        }
    }

    // オブジェクトを生成し、配置する
    for (auto& objectData : levelData->objects) {
        // 3Dオブジェクトを生成
        Object3d* newObject3d = new Object3d();
        newObject3d->Init();
        newObject3d->SetModel(objectData.filename + ".obj");
        newObject3d->SetTransform(objectData.transform);
        // 配列に追加
        objects.push_back(newObject3d);
    }
}

void Loader::LoadAllJsonFile(const std::string kDefaultBaseDirectory, const std::string fileName, const std::string sceneType, std::vector<Object3d*>& objects)
{
    // フルパスを生成
    const std::string fullpath = kDefaultBaseDirectory + "/" + fileName + ".json";

    // ファイルストリームを作成
    std::ifstream file;

    // ファイルを開く
    file.open(fullpath);
    // ファイルオープン失敗をチェック
    if (file.fail()) {
        assert(0);
    }

    // JSONデータを格納する変数
    nlohmann::json deserialized;

    // ファイルからJSONデータを読み込む
    file >> deserialized;

    // JSONデータの形式をチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("scenes"));

    // シーンを探索して目的のシーンを見つける
    nlohmann::json selectedScene;
    bool sceneFound = false;

    for (auto& scene : deserialized["scenes"]) {
        if (scene.contains("type") && scene["type"] == sceneType) {
            selectedScene = scene;
            sceneFound = true;
            break;
        }
    }

    // 指定されたシーンが見つからない場合はエラー
    assert(sceneFound);

    // レベルデータを格納するインスタンスを生成
    LevelData* levelData = new LevelData();

    // シーン内のオブジェクトを処理
    for (auto& object : selectedScene["objects"]) {
        assert(object.contains("type"));

        // オブジェクトの種別を取得
        std::string type = object["type"].get<std::string>();

        // MESHオブジェクトの場合
        if (type == "MESH") {
            levelData->objects.emplace_back(LevelData::ObjectData{});
            LevelData::ObjectData& objectData = levelData->objects.back();

            if (object.contains("file_name")) {
                objectData.filename = object["file_name"];
            }

            // トランスフォームのパラメータを読み込む
            auto& transform = object["transform"];
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            objectData.transform.rotate.x = -(float)transform["rotation"][0];
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];

            // モデルをロード
            ModelManager::GetInstance()->LoadModel("Resources/game/", objectData.filename + ".obj");
        }

        // CAMERAオブジェクトの場合
        if (type == "CAMERA") {
            levelData->objects.emplace_back(LevelData::ObjectData{});
            LevelData::ObjectData& objectData = levelData->objects.back();

            auto& transform = object["transform"];
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            objectData.transform.rotate.x = -((float)transform["rotation"][0] - 3.141592f / 2.0f);
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];
        }

        // LIGHTオブジェクトなど他の種別に対応する場合、ここに追加
    }

    // オブジェクトを生成し、配置する
    for (auto& objectData : levelData->objects) {
        Object3d* newObject3d = new Object3d();
        newObject3d->Init();
        newObject3d->SetModel(objectData.filename + ".obj");
        newObject3d->SetTransform(objectData.transform);
        objects.push_back(newObject3d);
    }
}

/// <summary>
/// JSONファイルを読み込み、番号付きのオブジェクトを初期化する関数
/// </summary>
/// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ</param>
/// <param name="fileName">読み込むJSONファイル名（拡張子なし）</param>
/// <param name="objects">生成したオブジェクトを格納するベクター</param>
void Loader::LoadJsonFileNumber(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects) {
    // フルパスを生成
    const std::string fullpath = kDefaultBaseDirectory + "/" + fileName + ".json";

    // ファイルストリームを作成
    std::ifstream file;

    // ファイルを開く
    file.open(fullpath);
    // ファイルオープン失敗をチェック
    if (file.fail()) {
        assert(0);
    }

    // JSONデータを格納する変数
    nlohmann::json deserialized;

    // ファイルからJSONデータを読み込む
    file >> deserialized;

    // 正しいレベルデータファイルかチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    // "name"フィールドを取得
    std::string name = deserialized["name"].get<std::string>();
    // "scene"であることを確認
    assert(name.compare("scene") == 0);

    // レベルデータを格納するインスタンスを生成
    LevelData* levelData = new LevelData();

    // "objects"配列内の全オブジェクトを走査
    for (nlohmann::json& object : deserialized["objects"]) {
        assert(object.contains("type"));

        // オブジェクトの種別を取得
        std::string type = object["type"].get<std::string>();

        // MESHオブジェクトの場合
        if (type.compare("MESH") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            if (object.contains("file_name")) {
                // ファイル名を取得
                objectData.filename = object["file_name"];
            }

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -(float)transform["rotation"][0];
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];
        }

        // 子オブジェクトが存在する場合（未実装）
        if (object.contains("children")) {
            // TODO: 再帰的に子オブジェクトを処理する
        }

        // CAMERAオブジェクトの場合
        if (type.compare("CAMERA") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -((float)transform["rotation"][0] - 3.141592f / 2.0f);
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];
        }
    }

    // オブジェクトを生成し、配置する
    for (auto& objectData : levelData->objects) {
        // 3Dオブジェクトを生成
        Object3d* newObject3d = new Object3d();
        newObject3d->Init();
        newObject3d->SetModel(objectData.filename + ".obj");
        newObject3d->SetTransform(objectData.transform);
        // 配列に追加
        objects.push_back(newObject3d);
    }
}

/// <summary>
/// JSONファイルを読み込み、テキストオブジェクトを初期化する関数
/// </summary>
/// <param name="kDefaultBaseDirectory">デフォルトのベースディレクトリ</param>
/// <param name="fileName">読み込むJSONファイル名（拡張子なし）</param>
/// <param name="objects">生成したオブジェクトを格納するベクター</param>
void Loader::LoadJsonFileText(const std::string kDefaultBaseDirectory, const std::string fileName, std::vector<Object3d*>& objects) {
    // フルパスを生成
    const std::string fullpath = kDefaultBaseDirectory + "/" + fileName + ".json";

    // ファイルストリームを作成
    std::ifstream file;

    // ファイルを開く
    file.open(fullpath);
    // ファイルオープン失敗をチェック
    if (file.fail()) {
        assert(0);
    }

    // JSONデータを格納する変数
    nlohmann::json deserialized;

    // ファイルからJSONデータを読み込む
    file >> deserialized;

    // 正しいレベルデータファイルかチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    // "name"フィールドを取得
    std::string name = deserialized["name"].get<std::string>();
    // "scene"であることを確認
    assert(name.compare("scene") == 0);

    // レベルデータを格納するインスタンスを生成
    LevelData* levelData = new LevelData();

    // "objects"配列内の全オブジェクトを走査
    for (nlohmann::json& object : deserialized["objects"]) {
        assert(object.contains("type"));

        // オブジェクトの種別を取得
        std::string type = object["type"].get<std::string>();

        // MESHオブジェクトの場合
        if (type.compare("MESH") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            if (object.contains("file_name")) {
                // ファイル名を取得
                objectData.filename = object["file_name"];
            }

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -(float)transform["rotation"][0];
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];

            // テキスト用モデルをロード
            ModelManager::GetInstance()->LoadModel("Resources/game/Text/", objectData.filename + ".obj");
        }

        // 子オブジェクトが存在する場合（未実装）
        if (object.contains("children")) {
            // TODO: 再帰的に子オブジェクトを処理する
        }

        // CAMERAオブジェクトの場合
        if (type.compare("CAMERA") == 0) {
            // オブジェクトデータを追加
            levelData->objects.emplace_back(LevelData::ObjectData{});
            // 追加したオブジェクトデータへの参照を取得
            LevelData::ObjectData& objectData = levelData->objects.back();

            // トランスフォームのパラメータを読み込む
            nlohmann::json& transform = object["transform"];
            // 平行移動
            objectData.transform.translate.x = (float)transform["translation"][0];
            objectData.transform.translate.y = (float)transform["translation"][2];
            objectData.transform.translate.z = (float)transform["translation"][1];
            // 回転角度
            objectData.transform.rotate.x = -((float)transform["rotation"][0] - 3.141592f / 2.0f);
            objectData.transform.rotate.y = -(float)transform["rotation"][2];
            objectData.transform.rotate.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.transform.scale.x = (float)transform["scaling"][0];
            objectData.transform.scale.y = (float)transform["scaling"][1];
            objectData.transform.scale.z = (float)transform["scaling"][2];
        }
    }

    // オブジェクトを生成し、配置する
    for (auto& objectData : levelData->objects) {
        // 3Dオブジェクトを生成
        Object3d* newObject3d = new Object3d();
        newObject3d->Init();
        newObject3d->SetModel(objectData.filename + ".obj");
        newObject3d->SetTransform(objectData.transform);
        // 配列に追加
        objects.push_back(newObject3d);
    }
}
