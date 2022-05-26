#include "Model.h"
#include "FrameWork.h"
#include "Graphics/SkinnedMesh.h"
#include "ResourceManager.h"


Model::Model(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate, BOOL frontCounterClockwise) {
	skinnedMesh = ResourceManager::Instance().LoadModelResource(fbxFilename, triangulate, frontCounterClockwise);
	modelFilename = fbxFilename;

	// ノード
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
	//再生中じゃないなら処理しない
	if (!IsPlayAnimatimon()) return;
	// 再生中断中
	if (stopAnimation)return;

	// ブレンド率の計算
	float blendRate = 1.0f;
	if (animationBlendTime < animationBlendSeconds) {
		animationBlendTime += elapsedTime;
		if (animationBlendTime >= animationBlendSeconds) {
			animationBlendTime = animationBlendSeconds;
		}
		blendRate = animationBlendTime / animationBlendSeconds;
	}

	// 指定のアニメーションデータを取得
	SkinnedMesh::Animation& animation = skinnedMesh.get()->animationClips[animationIndex];
	std::vector<SkinnedMesh::Animation::Keyframe>& keyframes = animation.sequence;

	// アニメーションデータからキーフレームデータリストを取得
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex) {
		// 現在の時間がどのキーフレームの間にいるか判定する
		const SkinnedMesh::Animation::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const SkinnedMesh::Animation::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		//if (animationSeconds >= keyIndex / animation.samplingRate && animationSeconds < keyIndex / animation.samplingRate + 1) {
		if (animationSeconds >= keyframe0.seconds && animationSeconds < keyframe1.seconds) {
			// 再生時間とキーフレームの時間から補完率を算出する
			float rate = (animationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
			//float rate = (animationSeconds - keyIndex / animation.samplingRate) / (keyIndex / animation.samplingRate + 1 - keyIndex / animation.samplingRate);
			int nodeCount = static_cast<int>(keyframe.nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
				// ２つのキーフレーム間の補完計算
				const SkinnedMesh::Animation::Keyframe::Node& key0 = keyframe0.nodes.at(nodeIndex);
				const SkinnedMesh::Animation::Keyframe::Node& key1 = keyframe1.nodes.at(nodeIndex);
				SkinnedMesh::Animation::Keyframe::Node& node = keyframe.nodes[nodeIndex];

				// ブレンド補完処理
				if (blendRate < 1.0f) {
					// 現在の姿勢と次のキーフレームとの姿勢の補完
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scaling);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scaling);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotation);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotation);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translation);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translation);
					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, blendRate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, blendRate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, blendRate);
					// 計算結果をボーンに格納
					DirectX::XMStoreFloat3(&node.scaling, S);
					DirectX::XMStoreFloat4(&node.rotation, R);
					DirectX::XMStoreFloat3(&node.translation, T);
				}
				//通常の計算
				else {
					// 前のキーフレームと次のキーフレームの姿勢を補完
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scaling);
					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scaling);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotation);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotation);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translation);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translation);
					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);
					// 計算結果をボーンに格納
					DirectX::XMStoreFloat3(&node.scaling, S);
					DirectX::XMStoreFloat4(&node.rotation, R);
					DirectX::XMStoreFloat3(&node.translation, T);
				}
			}
			//break;
		}
	}




	// 時間経過
	animationSeconds += elapsedTime;

	//再生が終わったら
	if (animationSeconds >= animation.secondsLength) {
		//ループ再生なら戻す
		if (animationLoopFlag) {
			animationSeconds = 0.0f;
		}
		//再生を終了する
		else {
			animationEndFlag = true;
			animationSeconds = 0.0f;
		}
	}

	// 最終フレーム処理
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

// ノード検索
SkinnedMesh::Animation::Keyframe::Node* Model::FindNode(const char* name) {
	// 全てのノードを総当たりで名前比較する
	for (auto& node : keyframe.nodes) {
		if (::strcmp(node.name.c_str(), name) == 0) {
			return &node;
		}
	}
	// 見つからなかった
	return nullptr;
}