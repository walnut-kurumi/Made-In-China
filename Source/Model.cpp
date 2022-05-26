#include "Model.h"
#include "FrameWork.h"
#include "Graphics/SkinnedMesh.h"
#include "ResourceManager.h"


Model::Model(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate, BOOL frontCounterClockwise) {
	skinnedMesh = ResourceManager::Instance().LoadModelResource(fbxFilename, triangulate, frontCounterClockwise);
	modelFilename = fbxFilename;

	// �m�[�h
	const std::vector<SkinnedMesh::Scene::Node>& resNodes = skinnedMesh->GetScene().nodes;

	keyframe.nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < keyframe.nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = keyframe.nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.scaling = src.scaling;
		dst.rotation = src.rotation;
		dst.translation = src.translation;
	}

	UpdateTransform(transform);
}

Model::~Model() {
	anime.clear();
}

void Model::PlayAnimation(int index, bool loop) {
	animationIndex = index;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationSeconds = 0.0f;
	animationBlendTime = 0.0f;
	animationBlendSeconds = 0.2f;
	stopAnimation = false;
}


void Model::Begin(ID3D11DeviceContext* dc, Shader shader, bool wireframe, bool cullFront)
{
	skinnedMesh.get()->Begin(dc, shader, wireframe, cullFront);
}

void Model::Render(ID3D11DeviceContext* dc, const Vec4 materialColor) {
	skinnedMesh.get()->render(dc, this->transform, materialColor, &keyframe);
	skinnedMesh.get()->End(dc);
}

void Model::LoadAnimation(const char* fbxFilename, float samplingRate, int index) {
	//skinnedMesh.get()->LoadAnimation(fbxFilename, samplingRate, index);
	anime.emplace(index,  ResourceManager::Instance().LoadAnimationResource(modelFilename, fbxFilename, samplingRate, index));
	skinnedMesh.get()->animationClips.emplace(index, *anime[index].get());
}

bool Model::IsPlayAnimatimon() {
	if (animationIndex < 0) return false;
	return true;
}

void Model::AnimationStop(bool b) {
	stopAnimation = b;
}

void Model::UpdateAnimation(float elapsedTime) {
	//�Đ�������Ȃ��Ȃ珈�����Ȃ�
	if (!IsPlayAnimatimon()) return;
	// �Đ����f��
	if (stopAnimation)return;

	// �u�����h���̌v�Z
	float blendRate = 1.0f;
	if (animationBlendTime < animationBlendSeconds) {
		animationBlendTime += elapsedTime;
		if (animationBlendTime >= animationBlendSeconds) {
			animationBlendTime = animationBlendSeconds;
		}
		blendRate = animationBlendTime / animationBlendSeconds;
	}

	// �w��̃A�j���[�V�����f�[�^���擾
	SkinnedMesh::Animation& animation = skinnedMesh.get()->animationClips[animationIndex];
	std::vector<SkinnedMesh::Animation::Keyframe>& keyframes = animation.sequence;

	// �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex) {
		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
		const SkinnedMesh::Animation::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const SkinnedMesh::Animation::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		//if (animationSeconds >= keyIndex / animation.samplingRate && animationSeconds < keyIndex / animation.samplingRate + 1) {
		if (animationSeconds >= keyframe0.seconds && animationSeconds < keyframe1.seconds) {
			// �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
			float rate = (animationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
			//float rate = (animationSeconds - keyIndex / animation.samplingRate) / (keyIndex / animation.samplingRate + 1 - keyIndex / animation.samplingRate);
			int nodeCount = static_cast<int>(keyframe.nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
				// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
				const SkinnedMesh::Animation::Keyframe::Node& key0 = keyframe0.nodes.at(nodeIndex);
				const SkinnedMesh::Animation::Keyframe::Node& key1 = keyframe1.nodes.at(nodeIndex);
				SkinnedMesh::Animation::Keyframe::Node& node = keyframe.nodes[nodeIndex];

				// �u�����h�⊮����
				if (blendRate < 1.0f) {
					// ���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scaling);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scaling);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotation);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotation);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translation);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translation);
					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, blendRate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, blendRate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, blendRate);
					// �v�Z���ʂ��{�[���Ɋi�[
					DirectX::XMStoreFloat3(&node.scaling, S);
					DirectX::XMStoreFloat4(&node.rotation, R);
					DirectX::XMStoreFloat3(&node.translation, T);
				}
				//�ʏ�̌v�Z
				else {
					// �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scaling);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scaling);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotation);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotation);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translation);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translation);
					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);
					// �v�Z���ʂ��{�[���Ɋi�[
					DirectX::XMStoreFloat3(&node.scaling, S);
					DirectX::XMStoreFloat4(&node.rotation, R);
					DirectX::XMStoreFloat3(&node.translation, T);
				}
			}
			//break;
		}
	}




	// ���Ԍo��
	animationSeconds += elapsedTime;

	//�Đ����I�������
	if (animationSeconds >= animation.secondsLength) {
		//���[�v�Đ��Ȃ�߂�
		if (animationLoopFlag) {
			animationSeconds = 0.0f;
		}
		//�Đ����I������
		else {
			animationEndFlag = true;
			animationSeconds = 0.0f;
		}
	}

	// �ŏI�t���[������
	if (animationEndFlag) {
		animationEndFlag = false;
		animationIndex = -1;
		return;
	}
}

void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform) {
	size_t nodeCount = keyframe.nodes.size();
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
		SkinnedMesh::Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z);

		int64_t parentIndex = skinnedMesh->GetScene().nodes.at(nodeIndex).parentIndex;
		DirectX::XMMATRIX P = parentIndex < 0
			? DirectX::XMMatrixIdentity()
			: DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parentIndex).globalTransform);

		DirectX::XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
	}
	this->transform = transform;
}

// �m�[�h����
SkinnedMesh::Animation::Keyframe::Node* Model::FindNode(const char* name) {
	// �S�Ẵm�[�h�𑍓�����Ŗ��O��r����
	for (auto& node : keyframe.nodes) {
		if (::strcmp(node.name.c_str(), name) == 0) {
			return &node;
		}
	}
	// ������Ȃ�����
	return nullptr;
}