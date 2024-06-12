#include "Object3d.h"
#include "Modelmanager.h"
#include "Object3dCommon.h"
void Object3d::Init()
{

	WinAPI* sWinAPI = WinAPI::GetInstance();
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	worldTransform_.Initialize();

	//�o�b�t�@���\�[�X
	// �f�[�^����������
	wvpData = nullptr;
	// WVP�p�̃��\�[�X�����BMatrix4x4 1���̃T�C�Y��p�ӂ���
	wvpResource = Mesh::CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));
	// �������ނ��߂̃A�h���X���擾
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//�P�ʍs�����������ł���
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();
	// �J�����p
	cameraForGPUData_ = nullptr;
	cameraForGPUResource_ = Mesh::CreateBufferResource(directXCommon->GetDevice(), sizeof(CameraForGPU));
	// �������ނ��߂̃A�h���X���擾
	cameraForGPUResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData_));

	cameraForGPUData_->worldPosition = { 1.0f,1.0f,-5.0f };

}

void Object3d::Update()
{
	worldTransform_.UpdateMatrix();
	if (model_) {
		model_->Update();
	}


}

void Object3d::Draw(uint32_t texture, Camera* camera)
{
	PSO* pso = PSO::GatInstance();
	DirectXCommon* directXCommon = DirectXCommon::GetInstance();
	cameraForGPUData_->worldPosition = camera->GetTransform().translate;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldTransform_.matWorld_, camera->GetViewprojectionMatrix());

	//directionalLightData->direction =  Normalize(directionalLightData->direction);
	directXCommon->GetCommandList()->SetGraphicsRootSignature(pso->GetProperty().rootSignature.Get());
	directXCommon->GetCommandList()->SetPipelineState(pso->GetProperty().graphicsPipelineState.Get());    //PSO��ݒ�
	//�`���ݒ�BPSO�ɐݒ肵�Ă�����̂Ƃ͂܂��ʁB�������̂�ݒ肷��ƍl���Ă����Ηǂ�
	directXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	directXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraForGPUResource_->GetGPUVirtualAddress());
	// 3D���f�������蓖�Ă��Ă���Ε`�悷��
	if (model_) {
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldTransform_.matWorld_;
		model_->Draw(texture, { { 1.0f,1.0f,1.0f,1.0f },false
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

ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData; // �\�z����MataData
	std::vector<Vector4> positions; // �ʒu
	std::vector<Vector3> normals; // �@��
	std::vector<Vector2> texcoords; // �e�N�X�`�����W
	std::string line; // �t�@�C������ǂ�1�s���i�[�������

	std::ifstream file(directoryPath + "/" + filename); // �t�@�C�����J��
	assert(file.is_open()); // �Ƃ肠�����J���Ȃ�������~�߂�

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // �擪�̎��ʎq��ǂ�

		// identifier�ɉ���������
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
			// �ʂ͎O�p�`����B���̑��͖��Ή�
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// ���_�̗v�f�ւ�Index�́u�ʒu/UV/�@���v�Ŋi�[����Ă���̂ŁA��������Index���擾����
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// ��؂�ŃC���f�b�N�X��ǂ�ł���
					elementIndices[element] = std::stoi(index);

				}
				// �v�f�ւ�Index����A���ۂ̗v�f�̒l���擾���āA���_���\�z����
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
			// materialtemplateLibrary�t�@�C���̖��O���擾����
			std::string materialFilename;
			s >> materialFilename;
			// ��{�I��obj�t�@�C���Ɠ���K�w��mtl�͑��݂�����̂ŁA�f�B���N�g�����ƃt�@�C������n��
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

	}
	return modelData;
}

MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;// �\�z����MaterialData
	std::string line; // �t�@�C������ǂ�1�s�������̂��������
	std::ifstream file(directoryPath + "/" + filename); // �t�@�C�����J��
	assert(file.is_open()); // �Ƃ肠�����J���Ȃ�������~�߂�

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifer�ɉ���������
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// �A�����ăt�@�C���p�X�ɂ���
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}

	}

	return materialData;
}
