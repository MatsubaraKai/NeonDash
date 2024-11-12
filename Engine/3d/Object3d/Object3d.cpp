#include "Object3d.h"
#include "Modelmanager.h"
#include "Object3dCommon.h"
/**
* @file Object3d.cpp
* @brief 3Dオブジェクトを管理するクラス
*/
void Object3d::Init()
{

	WinAPI* sWinAPI = WinAPI::GetInstance();
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	worldTransform_.Initialize();
	
	//バッファリソース
	// データを書き込む
	wvpData = nullptr;
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource = Mesh::CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでいく
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();
	// カメラ用
	cameraForGPUData_ = nullptr;
	cameraForGPUResource_ = Mesh::CreateBufferResource(directXCommon->GetDevice(), sizeof(CameraForGPU));
	//// 書き込むためのアドレスを取得
	cameraForGPUResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData_));

	cameraForGPUData_->worldPosition = { 1.0f,1.0f,-5.0f };

}

void Object3d::Update()
{
	worldTransform_.UpdateMatrix();
	if (animationModel_) {
		animationModel_->Update();
	}
	else if (model_) {
		model_->Update();
	}


}

void Object3d::Draw(uint32_t texture, Camera* camera)
{
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	if (animationModel_) {
		PSOAnimationModel* pso = PSOAnimationModel::GetInstance();
		directXCommon->GetCommandList()->SetGraphicsRootSignature(pso->GetProperty().rootSignature.Get());
		directXCommon->GetCommandList()->SetPipelineState(pso->GetProperty().graphicsPipelineState.Get());    //PSOを設定
	}
	else if (model_) {
		PSO* pso = PSO::GetInstance();

		directXCommon->GetCommandList()->SetGraphicsRootSignature(pso->GetProperty().rootSignature.Get());
		directXCommon->GetCommandList()->SetPipelineState(pso->GetProperty().graphicsPipelineState.Get());    //PSOを設定
		
	}
	else if (skybox_) {
		PSOSkybox* pso = PSOSkybox::GetInstance();
		directXCommon->GetCommandList()->SetGraphicsRootSignature(pso->GetProperty().rootSignature.Get());
		directXCommon->GetCommandList()->SetPipelineState(pso->GetProperty().graphicsPipelineState.Get());    //PSOを設定
	}

	cameraForGPUData_->worldPosition = camera->GetTransform().translate;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldTransform_.matWorld_, camera->GetViewprojectionMatrix());


	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	directXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraForGPUResource_->GetGPUVirtualAddress());
	// 3Dモデルが割り当てられていれば描画する
	if (animationModel_) {
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldTransform_.matWorld_;
		animationModel_->Draw(texture, { { 1.0f,1.0f,1.0f,1.0f },false
			}, { { 1.0f,1.0,1.0,1.0f } ,{ 0.0f,-1.0f,0.0f },0.5f }, mapTexture_);
	}
	else if (model_) {
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldTransform_.matWorld_;
		model_->Draw(texture, { { 1.0f,1.0f,1.0f,1.0f },false
			}, { { 1.0f,1.0,1.0,1.0f } ,{ 0.0f,-1.0f,0.0f },0.5f }, mapTexture_);
	}
	else if (skybox_) {
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldTransform_.matWorld_;
		skybox_->Draw(texture, { { 1.0f,1.0f,1.0f,1.0f },false
			}, { { 1.0f,1.0,1.0,1.0f } ,{ 0.0f,-1.0f,0.0f },0.5f });
	}
}

void Object3d::Release()
{

}

void Object3d::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::SetAnimationModel(const std::string& filePath)
{
	animationModel_ = ModelManager::GetInstance()->FindAnimationModel(filePath);
}

void Object3d::SetTransform(Transform transform)
{
	worldTransform_.translation_ = transform.translate;
	worldTransform_.rotation_ = transform.rotate;
	worldTransform_.scale_ = transform.scale;

}

ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData; // 構築するMataData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.z *= -1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;

			texcoord.y *= -1.0f;// -texcoord.y; //- texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.z *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);

				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//position.x *= -1.0f;
				//texcoord.y = 1.0f - texcoord.y;
				//normal.x *= -1.0f;

				VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);

				triangle[faceVertex] = { position,texcoord,normal };

			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		}
		else if (identifier == "mtllib") {
			// materialtemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

	}
	return modelData;
}

MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;// 構築するMaterialData
	std::string line; // ファイルから読んだ1行をかくのうするもの
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identiferに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}

	}

	return materialData;
}

void Object3d::ModelDebug(const char* name)
{
	//#ifdef _DEBUG

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.2f, 0.7f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.1f, 0.3f, 0.5f));
	ImGui::Begin("model");

	if (ImGui::TreeNode(name))
	{
		float translate[3] = { worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z };
		ImGui::DragFloat3("transform", translate, 0.01f);
		worldTransform_.translation_ = { translate[0],translate[1],translate[2] };

		float rotate[3] = { worldTransform_.rotation_.x , worldTransform_.rotation_.y, worldTransform_.rotation_.z };
		ImGui::DragFloat3("rotate", rotate, 0.01f);
		worldTransform_.rotation_ = { rotate[0],rotate[1],rotate[2] };

		float scale[3] = { worldTransform_.scale_.x, worldTransform_.scale_.y, worldTransform_.scale_.z };
		ImGui::DragFloat3("scale", scale, 0.01f);
		worldTransform_.scale_ = { scale[0],scale[1],scale[2] };
		ImGui::TreePop();
	}
	worldTransform_.UpdateMatrix();
	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	//#endif // _DEBUG

}
