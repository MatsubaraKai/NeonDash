#include "ModelManager.h"

void ModelManager::init()
{
	// �����Ŏ󂯎���ă����o�ϐ��ɋL�^����
	dxCommon_ = DirectXCommon::GetInstance();
}

void ModelManager::Update()
{
}

void ModelManager::Release()
{
}

void ModelManager::Finalize()
{
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filePath)
{
	// �ǂݍ��ݍς݃��f��������
	if (models.contains(filePath)) {
		//  �ǂݍ��ݍς݂Ȃ瑁��return
		return;
	}
	// ���f���̐����ƃt�@�C���ǂݍ��݁A������
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(directoryPath, filePath,
		{ { 1.0f,1.0f,1.0f,1.0 },true
		});
	models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	// �ǂݍ��ݍς݃��f��������
	if (models.contains(filePath)) {
		// �ǂݍ��݃��f����߂�l�Ƃ���return
		return models.at(filePath).get();
	}

	// �t�@�C������v����
	return nullptr;
}

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}
