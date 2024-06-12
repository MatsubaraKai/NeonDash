#include "PostProcess.h"
#include "PSOCopyImage.h"
#include "Mesh.h"
#include <SRVManager.h>
PostProcess::PostProcess()
{
}
void PostProcess::Init()
{
	// ���ۂɒ��_���\�[�X�����
	materialResource = Mesh::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(PostMaterial));

	/*materialBufferView = CreateBufferView();;*/
	// ���_���\�[�X�Ƀf�[�^����������
	materialData = nullptr;
	// �������ނ��߂̃A�h���X���擾
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->projectionInverse = camera_->GetViewprojectionMatrix();


}

void PostProcess::Update()
{
}

void PostProcess::Draw() {
	materialData->projectionInverse = camera_->GetViewprojectionMatrix();
	PSOCopyImage* pso_ = PSOCopyImage::GatInstance();
	DirectXCommon* sDirectXCommon = DirectXCommon::GetInstance();
	sDirectXCommon->GetCommandList()->SetGraphicsRootSignature(pso_->GetProperty().rootSignature.Get());
	sDirectXCommon->GetCommandList()->SetPipelineState(pso_->GetProperty().graphicsPipelineState.Get());    //PSO��ݒ�
	//sDirectXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_); // VBV��ݒ�
	//sDirectXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
	//�`���ݒ�BPSO�ɐݒ肵�Ă�����̂Ƃ͂܂��ʁB�������̂�ݒ肷��ƍl���Ă����Ηǂ�
	sDirectXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// �}�e���A��CBuffer�̏ꏊ��ݒ�
	// SRV ��DescriptorTable�̐擪��ݒ�B2��rootParameter[2]�ł���B
	sDirectXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, SRVManager::GetInstance()->GetGPUDescriptorHandle(sDirectXCommon->GetRenderIndex()));
	sDirectXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->GetGPUDescriptorHandle(sDirectXCommon->GetDepthIndex()));
	// �}�e���A��CBuffer�̏ꏊ��ݒ�
	sDirectXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(2, materialResource->GetGPUVirtualAddress());
	// �`��iDrawCall/�h���[�R�[���j
	//sDirectXCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);
	sDirectXCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}
