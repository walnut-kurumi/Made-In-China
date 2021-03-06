#pragma once

#include <map>
#include <memory>
#include <vector>
#include "Graphics/SkinnedMesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Vec.h"
#include "OutlineFromRasterizer.h"

class Model
{
public:
	Model(ID3D11Device* device, const char* fbxFilename, bool triangulate = false, float samplingRate = 0, BOOL frontCounterClockwise = true);
	~Model();

	// アニメーション再生
	void PlayAnimation(int index, bool loop);

	// アニメーション更新
	void UpdateAnimation(float elapsedTime);
	// 更新したNodeを計算
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// アニメーション読み込み格納
	void LoadAnimation(const char* fbxFilename, float samplingRate, int index);

	bool IsPlayAnimatimon();
	void AnimationStop(bool b);

	// ノード検索
	SkinnedMesh::Animation::Keyframe::Node* FindNode(const char* name);
	
	void Begin(ID3D11DeviceContext* dc, Shader shader, bool wireframe = false, bool cullFront = false);

	void Render(ID3D11DeviceContext* dc, const Vec4 materialColor = { 1.0f, 1.0f, 1.0f, 1.0f });// , const Animation::Keyframe* keyframe = nullptr);

	const std::vector<SkinnedMesh::Mesh>& GetMeshs() const
	{
		return skinnedMesh.get()->GetMeshs();
	}
	SkinnedMesh* GetSkinnedMeshs()
	{
		return skinnedMesh.get();
	}
	//const std::vector<SkinnedMesh::Animation>& getAnimation()
	//{
	//	return model.get()->GetAnimation();
	//}
	float GetCurrentAnimationSeconds() {
		return animationSeconds;
	}
	// 再生中か否か
	const bool GetPlay() { return stopAnimation; }

private:
	std::shared_ptr<SkinnedMesh> skinnedMesh;
	std::unordered_map<int, std::shared_ptr<SkinnedMesh::Animation>> anime;
	DirectX::XMFLOAT4X4 transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	SkinnedMesh::Animation::Keyframe keyframe{};

	int animationIndex = -1;
	bool animationLoopFlag = false;
	bool animationEndFlag = false;
	float animationSeconds = 0.0f;
	float animationBlendTime = 0.0f;
	float animationBlendSeconds = 0.0f;
	bool stopAnimation = false;

	const char* modelFilename{};
};
