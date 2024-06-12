#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

/*----------------------------------------------------------
   ���̃N���X�̓V���O���g���p�^�[�������ɐ݌v����
--------------------------------------------------------------*/

class SRVManager
{
public:
	void Init();

	static uint32_t Allocate();
private:
	SRVManager() = default;
	~SRVManager() = default;
	const SRVManager& operator=(const SRVManager&) = delete;
public:

	static SRVManager* GetInstance();
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_.Get(); };
	// �ő�SRV���i�ő�e�N�X�`�������j
	static const uint32_t kMaXSRVCount;
	// SRV�p�̃f�X�N���v�^�T�C�Y
	static uint32_t descriptorSize_;
	// SRV�p�f�X�N���v�^�q�[�v
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	// ���Ɏg�p����SRV�C���f�b�N�X
	static uint32_t useIndex_;

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	// SRV�����i�e�N�X�`���p�j
	static void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// SRV�����iStructured Buffer�p�j
	static void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	static void CreateSRVRenderTexture(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// SRV�����iDepth�p�j
	static void CreateSRVDepth(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	//static void CreateSRVRenderTexture(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	void PreDraw();

	void SetGraphicsRootDescriptortable(UINT rootParameterIndex, uint32_t srvIndex);


};