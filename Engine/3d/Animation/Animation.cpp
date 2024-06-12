#include "Animation.h"
#include <cassert>
AnimationData Animation::LoadAnimationFile(const std::string& directoryPath, const std::string& filePath)
{
	AnimationData animation;// ������A�j���[�V����
	Assimp::Importer importer;
	std::string filePathA = directoryPath + "/" + filePath;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);// �A�j���[�V�������Ȃ�
	aiAnimation* animationAssimp = scene->mAnimations[0]; // �ŏ��̃A�j���[�V���������̗p�B������񕡐��Ή�����ɉz�������Ƃ͂Ȃ�
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // ���Ԃ̒P�ʂ�b�ɕϊ�
	AnimationCurve<NodeAnimation> keyframes;
	// assimp�ł͌X��Node��Animation��channel�Ɠǂ�ł���̂�channel���񂵂�NodeAnimation�̏����Ƃ��Ă���
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyFrameVector3 keyframe{};
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // �������b�ɕϊ�
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyFrameQuaternion keyframe{};
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // �������b�ɕϊ�
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyFrameVector3 keyframe{};
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // �������b�ɕϊ�
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}

	}
	return animation;
}

void Animation::Init(const std::string& directoryPath, const std::string& filePath)
{
	animation_ = LoadAnimationFile(directoryPath, filePath);

}

void Animation::Updata()
{
	animationTime += 1.0f / 60.0f;
	animationTime = std::fmod(animationTime, animation_.duration); // �Ō�܂ōs������ŏ����烊�s�[�g�Đ��B���s�[�g���Ȃ��Ă��ʂɂ悢
	//NodeAnimation& rootAnimation = animation_.nodeAnimations[mode]
}

//Vector3 Animation::CalucateValue(const std::vector<KeyFrameVector3>& keyframes, float time)
//{
//	assert(!keyframes.empty()); // �L�[���Ȃ����͕̂Ԃ��l���킩��Ȃ��̂Ń_��
//	if (keyframes.size() == 1 || time <= keyframes[0].time) {// �L�[��1���A�������L�[�t���[���O�Ȃ�ŏ��̒l�Ƃ���
//		return keyframes[0].value;
//	}
//
//	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
//		size_t nextIndex = index + 1;
//		// index��nextIndex��2��keyframe���擾���Ĕ͈͓��Ɏ��������邩�𔻒�
//		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
//			// �͈͓����Ԃ���
//			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
//			return Lerp();
//		}
//	}
//	
//}
