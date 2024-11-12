#include "Sprite.h"
#include "WinAPI.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "mathFunction.h"
#include "Mesh.h"
#include "SRVManager.h"
#include "TextureManager.h"
/**
* @file Sprite.cpp
* @brief Sprite
*/

Sprite::Sprite() {};
Sprite::~Sprite() {};

void Sprite::Init(const Vector2& pos, const Vector2& size, const Vector2& anchorPoint, const Vector4& color, const std::string& filePath) {
	sWinAPI = WinAPI::GetInstance();
	sDirectXCommon = DirectXCommon::GetInstance();

	//rootParamerters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescripterTableを使う
	//rootParamerters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	//rootParamerters[0].DescriptorTable.pDescriptorRanges = descriptorRange_; // Tableの中身の配列を指定
	//rootParamerters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_); // Tableで利用する数

	// Sprite用の頂点リソースを作る
	vertexResourceSprite_ = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(VertexData) * 4);

	// リソースを先頭のアドレスから使う
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);

	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));

	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;
	// 1枚目の三角形
	vertexDataSprite_[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexDataSprite_[1].position = { left,top,0.0f,1.0f }; // 左上
	vertexDataSprite_[2].position = { right,bottom,0.0f,1.0f }; // 右下
	vertexDataSprite_[3].position = { right,top,0.0f,1.0f }; // 右上

	// 1枚目の三角形
	vertexDataSprite_[0].texcorrd = { 0.0f,1.0f };
	vertexDataSprite_[1].texcorrd = { 0.0f,0.0f };
	vertexDataSprite_[2].texcorrd = { 1.0f,1.0f };
	vertexDataSprite_[3].texcorrd = { 1.0f,0.0f };
	// 実際に頂点リソースを作る
	materialResource = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(Material));

	/*materialBufferView = CreateBufferView();;*/
	// 頂点リソースにデータを書き込む
	materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色のデータを変数から読み込み
	materialData->color = color;
	materialData->uvTransform = MakeIdentity4x4();



	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResouceSprite = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(TransformationMatrix));
	// 書き込むためのアドレスを取得
	transformationMatrixResouceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	// 単位行列を書き込んでおく
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();

	// Transform変数の初期化
	transform_ = { {size.x,size.y,1.0f},{0.0f,0.0f,0.0f},{pos.x,pos.y ,0.0f} };

	indexResourceSprite = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;

	transformUv = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	const DirectX::TexMetadata& metadata_ =
		TextureManager::GetInstance()->GetMetaData(filePath);
	originSize_ = { static_cast<float>(metadata_.width),static_cast<float>(metadata_.height) };

};
void Sprite::Update() {
	/*transform_.translate = { position_.x,position_.y ,0.0f };
	transform_.scale = { size_.x,size_.y,1.0f };*/
	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;


	// 1枚目の三角形
	vertexDataSprite_[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexDataSprite_[1].position = { left,top,0.0f,1.0f }; // 左上
	vertexDataSprite_[2].position = { right,bottom,0.0f,1.0f }; // 右下
	vertexDataSprite_[3].position = { right,top,0.0f,1.0f }; // 右上


	float tex_left = textureleftTop_.x / originSize_.x;
	float tex_right = (textureleftTop_.x + textureSize_.x) / originSize_.x;
	float tex_top = textureleftTop_.y / originSize_.y;
	float tex_bottom = (textureleftTop_.y + textureSize_.y) / originSize_.y;

	// 1枚目の三角形
	vertexDataSprite_[0].texcorrd = { tex_left,tex_bottom };
	vertexDataSprite_[1].texcorrd = { tex_left,tex_top };
	vertexDataSprite_[2].texcorrd = { tex_right,tex_bottom };
	vertexDataSprite_[3].texcorrd = { tex_right,tex_top };
	//// 1枚目の三角形
	//vertexDataSprite_[0].texcorrd = { 0.0f,1.0f };
	//vertexDataSprite_[1].texcorrd = { 0.0f,0.0f };
	//vertexDataSprite_[2].texcorrd = { 1.0f,1.0f };
	//vertexDataSprite_[3].texcorrd = { 1.0f,0.0f };
};


void Sprite::Draw(uint32_t texture, const Vector4& color) {
	pso_ = PSOSprite::GetInstance();
	materialData->color = color;
	// Sprite用のWorldViewProjectMatrixを作る
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinAPI::kClientWidth_), float(WinAPI::kClientHeight_), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
	sDirectXCommon->GetCommandList()->SetGraphicsRootSignature(pso_->GetProperty().rootSignature.Get());
	sDirectXCommon->GetCommandList()->SetPipelineState(pso_->GetProperty().graphicsPipelineState.Get());    //PSOを設定
	sDirectXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_); // VBVを設定
	sDirectXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	sDirectXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	sDirectXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	// TransformationmatrixCBufferの場所を設定
	sDirectXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResouceSprite->GetGPUVirtualAddress());
	// SRV のDescriptorTableの先頭を設定。2はrootParameter[2]である。
	sDirectXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetGPUDescriptorHandle(texture));
	// 描画（DrawCall/ドローコール）
	//sDirectXCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);
	sDirectXCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
void Sprite::Release() {};

void Sprite::SpriteDebug(const char* name)
{
#ifdef _DEBUG
	ImGui::Begin("sprite");

	if (ImGui::TreeNode(name))
	{
		float translate[3] = { transform_.translate.x, transform_.translate.y, transform_.translate.z };
		ImGui::DragFloat3("transform", translate, 1.0f);
		transform_.translate = { translate[0],translate[1],translate[2] };

		float rotate[3] = { transform_.rotate.x , transform_.rotate.y, transform_.rotate.z };
		ImGui::DragFloat3("rotate", rotate, 0.01f);
		transform_.rotate = { rotate[0],rotate[1],rotate[2] };

		float scale[3] = { transform_.scale.x, transform_.scale.y, transform_.scale.z };
		ImGui::DragFloat3("scale", scale, 1.0f);
		transform_.scale = { scale[0],scale[1],scale[2] };
		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG

}

D3D12_VERTEX_BUFFER_VIEW Sprite::CreateBufferView() {
	D3D12_VERTEX_BUFFER_VIEW view{};

	//リソースの先頭のアドレスから使う
	view.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	view.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	view.StrideInBytes = sizeof(VertexData);


	return view;
};