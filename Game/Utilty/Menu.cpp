#include "Menu.h"
#include "Audio.h"

/// <summary>
/// メニューの初期化を行う関数
/// </summary>
/// <param name="Texture">メニューのテクスチャハンドル</param>
void Menu::Init(uint32_t Texture)
{
    // メニュー選択音のサウンドハンドルをロード
    Audiomenuhandle_ = Audio::SoundLoadWave("Resources/game/Audio/menu.wav");
    // メニューポジションのテクスチャハンドルを取得
    MENUPOSITIONtextureHandle = TextureManager::StoreTexture("Resources/game/menuposition.png");
    // メニューメディアのテクスチャハンドルを設定
    MENUMEDItextureHandle = Texture;
    // スプライトオブジェクトを生成
    menuSprite = new Sprite();
    menuSprite2 = new Sprite();
    // スプライトを初期化
    menuSprite->Init({ 0.0f, 0.0f }, { 1280.0f, 720.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, "Resources/menumedi.png");
    menuSprite2->Init({ 0.0f, 0.0f }, { 1280.0f, 720.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, "Resources/menuposition.png");
    // テクスチャサイズを設定
    menuSprite->SetTextureSize({ 1280.0f, 720.0f });
    menuSprite2->SetTextureSize({ 1280.0f, 720.0f });
    // マテリアルの設定
    material.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    material.enableLighting = true;
}

/// <summary>
/// メニューのテクスチャを変更する関数
/// </summary>
/// <param name="Texture">新しいテクスチャハンドル</param>
void Menu::ChangeTex(uint32_t Texture) {
    // メニューメディアのテクスチャハンドルを更新
    MENUMEDItextureHandle = Texture;
}

/// <summary>
/// メニューを描画する関数
/// </summary>
void Menu::Draw() {
    // メニューのスプライトを描画
    menuSprite->Draw(MENUMEDItextureHandle, material.color);
    menuSprite2->Draw(MENUPOSITIONtextureHandle, material.color);
}

/// <summary>
/// メニュー選択時の効果音を再生する関数
/// </summary>
void Menu::SE() {
    // メニュー選択音を再生
    Audio::SoundPlayWave(Audio::GetInstance()->GetIXAudio().Get(), Audiomenuhandle_, false, 0.4f);
}
