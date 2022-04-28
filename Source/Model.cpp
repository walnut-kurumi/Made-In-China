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
	anime.clear();
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
	skinnedMesh.get()->render(dc, this->transform, materialColor, &keyframe);
	skinnedMesh.get()->End(dc);
}

void Model::LoadAnimation(const char* fbxFilename, float samplingRate, int index)
{
	//skinnedMesh.get()->LoadAnimation(fbxFilename, samplingRate, index);
	anime.emplace(index,  ResourceManager::Instance().LoadAnimationResource(fbxFilename, samplingRate, index));
	skinnedMesh.get()->animationClips.emplace(index, *anime[index].get());
}

bool Model::IsPlayAnimatimon()
{
	if (animationIndex < 0) return false;
	//if (animationIndex >= skinnedMesh.get()->animationClips.size()) return false;
	return true;
}

void Model::UpdateAnimation(float elapsedTime)
{
	//再生中じゃないなら処理しない
	if (!IsPlayAnimatimon()) return;

	// ブレンド率の計算
	float blendRate = 1.0f;
	if (animationBlendTime < animationBlendSeconds) {
		animationBlendTime += elapsedTime;
		if (animationBlendTime >= animationBlendSeconds) {
			animationBlendTime = animationBlendSeconds;
		}
		blendRate = animationBlendTime / animationBlendSeconds;
		blendRate *= blendRate;
	}

	// 指定のアニメーションデータを取得
	SkinnedMesh::Animation& animation = skinnedMesh.get()->animationClips[animationIndex];
	keyframe = animation.sequence.at(static_cast<int>(animationSeconds * animation.samplingRate));
	// 時間経過
	animationSeconds += elapsedTime;

	//再生が終わったら
	if (animationSeconds > animation.secondsLength) {
		//ループ再生なら戻す
		if (animationLoopFlag) {
			animationSeconds = 0.0f;
		}
		//再生を終了する
		else {
			animationEndFlag = true;
			animationSeconds = animation.secondsLength;
		}
	}

	// 指定のアニメーションデータを取得
	/*if (!animationEndFlag)*/ 

	// 最終フレーム処理
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