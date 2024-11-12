#pragma once
#include <d3d12.h>
#include "DirectXCommon.h"
#include "PSOProperty.h"

/// <summary>
/// @file PSOSprite.h
/// @brief スプライト用のパイプラインステートオブジェクトを管理するクラスの宣言
/// </summary>

/// <summary>
/// スプライト用のパイプラインステートオブジェクトを管理するクラス
/// </summary>
class PSOSprite : public PSOProperty
{
public:
    /// <summary>
    /// シングルトンのインスタンスを取得する
    /// </summary>
    /// <returns>PSOSpriteクラスのインスタンス</returns>
    static PSOSprite* GetInstance();

    /// <summary>
    /// コンストラクタ
    /// </summary>
    PSOSprite() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~PSOSprite() = default;

    /// <summary>
    /// コピー代入演算子を削除（コピー不可）
    /// </summary>
    const PSOSprite& operator=(const PSOSprite&) = delete;

    /// <summary>
    /// 描画に関する設定をまとめる関数
    /// </summary>
    void CreatePipelineStateObject();

    /// <summary>
    /// シェーダーとリソースの関係を設定する関数
    /// </summary>
    void CreateRootSignature();

    /// <summary>
    /// 頂点シェーダーへ渡す頂点データの指定を行う関数
    /// </summary>
    void SetInputLayout();

    /// <summary>
    /// ピクセルシェーダーからの出力をどのように書き込むか設定する関数
    /// </summary>
    void SetBlendState();

    /// <summary>
    /// ラスタライザーに関する設定を行う関数
    /// </summary>
    void SetRasterizerState();

    /// <summary>
    /// デプスバッファを生成する関数
    /// </summary>
    void CreateDepth();

    /// <summary>
    /// PSOプロパティを取得する関数
    /// </summary>
    /// <returns>PSOPropertyオブジェクト</returns>
    PSOProperty GetProperty() { return property; }

private:
    /// <summary>
    /// 処理結果を格納するHRESULT
    /// </summary>
    HRESULT hr_;

    /// <summary>
    /// ルートシグネチャの記述子
    /// </summary>
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

    /// <summary>
    /// ルートパラメータの配列
    /// </summary>
    D3D12_ROOT_PARAMETER rootParameters[3] = {};

    /// <summary>
    /// スタティックサンプラーの記述子
    /// </summary>
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

    /// <summary>
    /// ディスクリプタレンジの配列
    /// </summary>
    D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};

    /// <summary>
    /// 入力要素の記述子の配列
    /// </summary>
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

    /// <summary>
    /// 入力レイアウトの記述子
    /// </summary>
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

    /// <summary>
    /// ブレンドステートの記述子
    /// </summary>
    D3D12_BLEND_DESC blendDesc{};

    /// <summary>
    /// ラスタライザーステートの記述子
    /// </summary>
    D3D12_RASTERIZER_DESC rasterizerDesc{};

    /// <summary>
    /// グラフィックスパイプラインステートの記述子
    /// </summary>
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

    /// <summary>
    /// デプスステンシルステートの記述子
    /// </summary>
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

    /// <summary>
    /// 深度ステンシルビュー（DSV）のハンドル
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

    /// <summary>
    /// PSOプロパティ
    /// </summary>
    PSOProperty property;
};
