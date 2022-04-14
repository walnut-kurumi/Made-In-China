#include "Model.h"
#include "FrameWork.h"
#include "Graphics/SkinnedMesh.h"
#include "ResourceManager.h"


Model::Model(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate, BOOL frontCounterClockwise)
{
	//skinnedMesh = std::make_unique<SkinnedMesh>(device, fbxFilename, triangulate);
	//skinnedMesh.get()->Init(device, frontCounterClockwise);
	skinnedMesh = ResourceManager::Instance().LoadModelResource(fbxFilename, triangulate, frontCounterClockwise);
}

Model::~Model()
{
}

void Model::PlayAnimation(int index, bool loop)
{
	animationIndex = index;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationSeconds = 0.0f;
}

void Model::Begin(ID3D11DeviceContext* dc, Shader shader, bool wireframe)
{
	skinnedMesh.get()->Begin(dc, shader, wireframe);
}

void Model::Render(ID3D11DeviceContext* dc, const Vec4 materialColor)  // , const Animation::Keyframe* keyframe = nullptr);
{
	skinnedMesh.get()->render(dc, this->transform, materialColor, keyframe);
	skinnedMesh.get()->End(dc);
}

void Model::LoadAnimation(const char* fbxFilename, float samplingRate, int index)
{
	//skinnedMesh.get()->LoadAnimation(fbxFilename, samplingRate, index);
	skin.emplace(index,  ResourceManager::Instance().LoadAnimationResource(fbxFilename, samplingRate, index));
	skinnedMesh.get()->animationClips.emplace(index, skin[index].get()->GetAnimes(index));
}

bool Model::IsPlayAnimatimon()
{
	if (animationIndex < 0) return false;
	//if (animationIndex >= skinnedMesh.get()->animationClips.size()) return false;
	return true;
}

void Model::UpdateAnimation(float elapsedTime)
{
	//�Đ�������Ȃ��Ȃ珈�����Ȃ�
	if (!IsPlayAnimatimon()) return;

	int frameIndex = 0;
	SkinnedMesh::Animation& animation = skinnedMesh.get()->animationClips[animationIndex];
	frameIndex = static_cast<int>(animationSeconds * animation.samplingRate);

	animationSeconds += elapsedTime;

	//�Đ����I�������
	if (frameIndex > animation.sequence.size() - 1) {
		//���[�v�Đ��Ȃ�߂�
		if (animationLoopFlag) {
			frameIndex = 0;
			animationSeconds = 0.0f;
		}
		//�Đ����I������
		else {
			animationEndFlag = true;
		}
	}

	//�Đ���
	if (!animationEndFlag) keyframe = &animation.sequence.at(frameIndex);

	// �ŏI�t���[������
	if (animationEndFlag) {
		animationEndFlag = false;
		animationIndex = -1;
		return;
	}
}

void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	this->transform = transform;
}