#pragma once
#include "Sprite.h"

/// <summary>
/// @file Menu.h
/// @brief メニュー画面のクラス宣言とその機能を提供します。
/// </summary>

/// <summary>
/// メニュー画面を管理するクラス。
/// </summary>
class Menu
{
public:
    /// <summary>
    /// メニューを初期化する関数。
    /// </summary>
    /// <param name="Texture">メニューのテクスチャハンドル。</param>
    void Init(uint32_t Texture);

    /// <summary>
    /// メニューのテクスチャを変更する関数。
    /// </summary>
    /// <param name="Texture">新しいテクスチャハンドル。</param>
    void ChangeTex(uint32_t Texture);

    /// <summary>
    /// メニュー選択時の効果音を再生する関数。
    /// </summary>
    void SE();

    /// <summary>
    /// メニューを描画する関数。
    /// </summary>
    void Draw();

private:
    /// <summary>
    /// メニューの背景スプライト。
    /// </summary>
    Sprite* menuSprite = nullptr;

    /// <summary>
    /// メニューの選択位置を示すスプライト。
    /// </summary>
    Sprite* menuSprite2 = nullptr;

    /// <summary>
    /// メニューのメディアテクスチャハンドル。
    /// </summary>
    uint32_t MENUMEDItextureHandle;

    /// <summary>
    /// メニューポジションのテクスチャハンドル。
    /// </summary>
    uint32_t MENUPOSITIONtextureHandle;

    /// <summary>
    /// メニューの効果音ハンドル。
    /// </summary>
    uint32_t Audiomenuhandle_;

    /// <summary>
    /// メニューのマテリアル情報。
    /// </summary>
    Material material;
};
