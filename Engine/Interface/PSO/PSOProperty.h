#pragma once
#include <d3d12.h>
#include "DirectXCommon.h"

/**
 * @file PSOProperty.h
 * @brief PSOProperty クラスと関連するブレンドモードの定義
 */
enum BlendMode {
	//!< ブレンドなし	
	kBlendModeNone,
	//!< 通常αブレンド。 デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	kBlendModeNormal,
	//!< 加算。 Src * SrcA + Dest * 1
	kBlendModeAdd,
	//!< 減算。 Dest * 1 - Src * SrcA
	kBlendModeSubtract,
	//!< 乗算。Src * (1 - Dest) + Dest* 1
	kBlendModeMultiply,
	//!< スクリーン。 Src * (1 - Dest) + Dest + 1
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

/**
 * @brief パイプラインステートオブジェクトのプロパティを保持するクラス
 */
class PSOProperty {
public:
	/**
	 * @brief PSO の各種コンポーネントを保持する構造体
	 */
	struct {
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState; ///< グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;         ///< ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;                    ///< シリアライズされたルートシグネチャのバイナリ
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;                        ///< シェーダーコンパイル時のエラーメッセージ
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;                 ///< コンパイル済みの頂点シェーダーのバイナリ
		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;                  ///< コンパイル済みのピクセルシェーダーのバイナリ
	};

protected:
	static int BlendNo_; ///< 現在のブレンドモードを示す静的メンバ変数
};