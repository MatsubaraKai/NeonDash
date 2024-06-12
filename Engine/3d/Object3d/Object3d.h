#pragma once
#include "PSOModel.h"
#include "ModelData.h"
#include "MaterialData.h"
#include "vertexData.h"

#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include <dxcapi.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <Material.h>
#include <Transform.h>
#include <Camera.h>
#include <WorldTransform.h>
#include "Model.h"
#include "Object3dCommon.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")
class Object3d
{
public:
	void Init();
	void Update();
	void Draw(uint32_t texture, Camera* camera);
	void Release();

public: // Setter
	void SetModel(Model* model) { model_ = model; }
	void SetModel(const std::string& filePath);
	void SetWorldTransform(const WorldTransform& worldtransform) { worldTransform_ = worldtransform; };
public: // Getter
	WorldTransform GetWorldTransform() { return worldTransform_; }
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
private:
	Object3dCommon* objectCommon_ = nullptr;

	HRESULT hr;
	// RootSignature�쐬
	Model* model_ = nullptr;

	/*�ړ��p*/
	// WVP�p�̃��\�[�X�����BMatrix4x4 1���̃T�C�Y��p�ӂ���
	TransformationMatrix* wvpData;
	Microsoft::WRL::ComPtr < ID3D12Resource> wvpResource;
	// ���_�o�b�t�@�r���[���쐬����
	D3D12_VERTEX_BUFFER_VIEW wvpBufferView{};
	Transform transformUv;

	/*�J�����p*/
	Microsoft::WRL::ComPtr < ID3D12Resource> cameraForGPUResource_;
	CameraForGPU* cameraForGPUData_;
	// �f�[�^����������
	WorldTransform worldTransform_;
};

