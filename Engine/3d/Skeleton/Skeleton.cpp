#include "Skeleton.h"
#include "Model.h"
#include "SRVManager.h"

void Skeleton::Update(SkeletonData& skeleton)
{

}

int32_t Skeleton::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size()); // ���ݓo�^����Ă鐔��Index��
	joint.parent = parent;
	joints.push_back(joint); // Skeleton��Joint��ɒǉ�
	for (const Node& child : node.children) {
		// �qIndex���쐬���A����Index�𑸂�
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].chaidren.push_back(childIndex);
	}
	// ���g��Index��Ԃ�
	return joint.index;
}

SkeletonData Skeleton::CreateSkeleton(const Node& rootNode)
{
	SkeletonData skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// ���O��index�̃}�b�s���O���s���A�N�Z�X���₷������
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}
	return skeleton;
}

SkinCluster Skeleton::CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const SkeletonData& skeleton,
	const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize)
{
	SkinCluster skinCluster;
	// particle�p��Resource���m��
	skinCluster.paletteResource = Mesh::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(),
		sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette, skeleton.joints.size() };// span���g���ăA�N�Z�X����悤�ɂ���
	uint32_t index = SRVManager::GetInstance()->Allocate();
	skinCluster.paletteSrvHandle.first = SRVManager::GetInstance()->GetCPUDescriptorHandle(
		index);
	skinCluster.paletteSrvHandle.second = SRVManager::GetInstance()->GetGPUDescriptorHandle(
		index);

	// palette�p��srv���쐬�BStructureBuffer�ŃA�N�Z�X�ł���悤�ɂ���
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{  };
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(skinCluster.paletteResource.Get(),
		&paletteSrvDesc, skinCluster.paletteSrvHandle.first);

	// influence�p��Resource���m�ہB���_����influence����ǉ��ł���悤�ɂ���
	skinCluster.influenceResource = Mesh::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(),
		sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };

	// Influence�p��VBV���쐬
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);


	// InverseBindPoseMatrix���i�[����ꏊ���쐬���āA�P�ʍs��Ŗ��߂�
	skinCluster.inverseBindposeMatrices.resize(skeleton.joints.size());
	std::generate(skinCluster.inverseBindposeMatrices.begin(), skinCluster.inverseBindposeMatrices.end(), MakeIdentity4x4);

	for (const auto& jointWeight : modelData.skinClusterData) { // Model��SkinCluster�̏��@�����
		auto it = skeleton.jointMap.find(jointWeight.first); // joitweight.first��joint���Ȃ̂ŁAskeleton�ɑΏۂƂȂ�joint���܂܂�Ă��邩���f
		if (it == skeleton.jointMap.end()) { // jointWeight.first���Ȃ̂ŁAskeleton�ɑΏۂƂȂ�joint���܂܂�Ă��邩���f
			continue;
		}
		// (*itr).second�ɂ�index�������Ă���̂ŁA�Y����index��inversePoseMatrix����
		skinCluster.inverseBindposeMatrices[(*it).second] = jointWeight.second.inverseBindPposeMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];// �Y����vertexIndex��influence�����Q�Ƃ��Ă���
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) { // �󂢂Ă���Ƃ���ɓ����
				if (currentInfluence.weights[index] == 0.0f) {// weight == 0���󂢂Ă����ԂȂ̂ŁA���̏ꏊ��weight��joint��index����
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}