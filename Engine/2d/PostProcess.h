#pragma once

#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include <dxcapi.h>
#include <fstream>
#include <sstream>
#include <cassert>

#include "function.h"
#include <wrl.h>
#include "Transform.h"
#include "VertexData.h"
#include "Vector4.h"
#include"Vector3.h"
#include"Vector2.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "DirectionLight.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "Camera.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")
struct PostMaterial {
	Matrix4x4 projectionInverse;
};
class PostProcess
{
public:



	PostProcess();
	~PostProcess();

	void Init();
	void Update();
	void Draw();
	void Release();
	void SetTextureManager(TextureManager* textureManager) {
		textureManager_ = textureManager;
	}
	D3D12_VERTEX_BUFFER_VIEW CreateBufferView();

	void SetCamera(Camera* camera) { camera_ = camera; }
private:

	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResourceSprite_ = nullptr;
	WinAPI* sWinAPI;
	TextureManager* textureManager_ = nullptr;
	// ���_�o�b�t�@�r���[���쐬����
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};

	VertexData* vertexDataSprite_ = nullptr;

	// Sprite�p��TransformationMatrix�p�̃��\�[�X�����BMatrix4x4 1���̃T�C�Y��p�ӂ���
	Microsoft::WRL::ComPtr < ID3D12Resource> transformationMatrixResouceSprite;
	// �f�[�^����������

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

	//D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};

	// RootParmeter�쐬�B�����ł��Ă��ł���̂Ŕz��B����͌��ʂP�����Ȃ̂Œ���1�̔z��
	//D3D12_ROOT_PARAMETER rootParamerters[1] = {};

	uint32_t* indexDataSprite;
	Microsoft::WRL::ComPtr < ID3D12Resource> indexResourceSprite;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};


	// ���ۂɒ��_���\�[�X�����
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource;
	// ���_�o�b�t�@�r���[���쐬����
	D3D12_VERTEX_BUFFER_VIEW materialBufferView{};
	// ���_���\�[�X�Ƀf�[�^����������
	PostMaterial* materialData;
	//ParticlePro particles_[kNumMaxInstance];
	std::list<Transform>  transforms_;
	// ���s�����p
	Microsoft::WRL::ComPtr < ID3D12Resource> directionalLightResource;

	// ���s�����p
	Microsoft::WRL::ComPtr < ID3D12Resource> depthStencilResource;
	// �f�[�^����������
	DirectionalLight* directionalLightData;
	Transform transformUv;

	uint32_t SRVIndex_;

	Camera* camera_ = nullptr;
};

