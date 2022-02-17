#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include "Shaders.h"

#include "Blender.h"

//SERIALIZATION
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>


#include <vector>
#include <unordered_map>
#include <string>
#include <fbxsdk.h>




namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v) {
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v) {
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v) {
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m) {
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}





class SkinnedMesh
{
private:
	//シェーダ関連
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		solidRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		wireRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	std::unique_ptr<Blender> blender;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>          rampSamplerState;

public:
	void Init(ID3D11Device* device, BOOL frontCounterClockwise);
	void Begin(ID3D11DeviceContext* dc, Shader shader, bool wire = false);
	void End(ID3D11DeviceContext* dc);
public:
	static const int MAX_BONE_INFLUENCES = 4;
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
		DirectX::XMFLOAT2 texcoord;
		float boneWeights[MAX_BONE_INFLUENCES] = { 1, 0, 0, 0 };
		uint32_t boneIndices[MAX_BONE_INFLUENCES]{};

		template<class T>
		void serialize(T& archive) {
			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};

	static const int MAX_BONES = 256;
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES] = { { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 } };
	};

	struct Ramp
	{
		DirectX::XMFLOAT4 ka;
		DirectX::XMFLOAT4 kd;
		DirectX::XMFLOAT4 ks;
		DirectX::XMFLOAT4 ambientColor = { 1.0f,1.0f,1.0f,1.0f };
		DirectX::XMFLOAT4 directionalLightColor = { 1.0f,1.0f,1.0f,1.0f };
	};


	//同じマテリアルを持つポリゴン集
	struct Subset
	{
		uint64_t materialUniqueId = 0;
		std::string materialName;
		uint32_t indexCount = 0;
		uint32_t startIndexLocation = 0;

		template<class T>
		void serialize(T& archive) {
			archive(materialUniqueId, materialName, indexCount, startIndexLocation);
		}
	};



	struct Scene
	{
		struct Node
		{
			uint64_t uniqueId = 0;
			std::string name;
			FbxNodeAttribute::EType attribute = FbxNodeAttribute::EType::eUnknown;
			int64_t parentIndex = -1;

			template<class T>
			void serialize(T& archive) {
				archive(uniqueId, name, attribute, parentIndex);
			}
		};
		std::vector<Node> nodes;
		int64_t indexof(uint64_t uniqueId) const {
			int64_t index = 0;
			for (const Node& node : nodes) {
				if (node.uniqueId == uniqueId) {
					return index;
				}
				++index;
			}
			return -1;
		}

		template<class T>
		void serialize(T& archive) {
			archive(nodes);
		}
	};

	struct Skeleton
	{
		struct Bone
		{
			uint64_t uniqueId = 0;
			std::string name;
			int64_t parentIndex = -1; // -1 : the bone is orphan
			int64_t nodeIndex = 0;
			// モデル(メッシュ)空間からボーン(ノード)シーンに変換するために使用されます
			DirectX::XMFLOAT4X4 offsetTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

			DirectX::XMFLOAT3 scaling = { 1, 1, 1 };
			DirectX::XMFLOAT4 rotation = { 0, 0, 0, 1 }; // Rotation quaternion
			DirectX::XMFLOAT3 translation = { 0, 0, 0 };

			bool isOrphan() const { return parentIndex < 0; };

			template<class T>
			void serialize(T& archive) {
				archive(uniqueId, name, parentIndex, nodeIndex, offsetTransform, scaling, rotation, translation);
			}
		};
		std::vector<Bone> bones;

		int64_t indexof(uint64_t uniqueId) const {
			int64_t index = 0;
			for (const Bone& bone : bones) {
				if (bone.uniqueId == uniqueId) return index;
				++index;
			}
			return -1;
		}

		template<class T>
		void serialize(T& archive) {
			archive(bones);
		}
	};


	struct Animation
	{
		std::string name;
		float samplingRate = 0;

		struct Keyframe
		{
			struct Node
			{
				// ローカル空間からグローバル空間への変換に使用
				// 全ての子から親の行列を合成したもの
				// （一回の行列計算でルートノードの座標に変換できる）
				DirectX::XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

				//ノードの変換データにはその親に関しての平行移動、回転、スケーリングのベクトルが含まれます			
				DirectX::XMFLOAT3 scaling = { 1, 1, 1 };
				DirectX::XMFLOAT4 rotation = { 0, 0, 0, 1 }; // Rotation quaternion
				DirectX::XMFLOAT3 translation = { 0, 0, 0 }; // 親ノードの座標における位置（このノードの座標の原点）
				// 上三つ合成→ 子~親    Local Transform 

				template<class T>
				void serialize(T& archive) {
					archive(globalTransform, scaling, rotation, translation);
				}
			};
			std::vector<Node> nodes;

			template<class T>
			void serialize(T& archive) {
				archive(nodes);
			}
		};
		std::vector<Keyframe> sequence;

		template<class T>
		void serialize(T& archive) {
			archive(name, samplingRate, sequence);
		}
	};



	struct Mesh
	{
		uint64_t uniqueId = 0;
		std::string name;
		int64_t nodeIndex = 0;

		// こいつがmodel行列
		DirectX::XMFLOAT4X4 defaultGlobalTransform = { 1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   0, 0, 0, 1 };

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Subset> subsets;
		Skeleton bindPose;

		//0:MIN, 1:MAX
		DirectX::XMFLOAT3 boundingBox[2] = {
			{ FLT_MAX, FLT_MAX, FLT_MAX },
			{ FLT_MIN, FLT_MIN, FLT_MIN }
		};
		
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

		template<class T>
		void serialize(T& archive) {
			archive(uniqueId, name, nodeIndex, subsets, defaultGlobalTransform, bindPose, boundingBox, vertices, indices);

		}
	};

	struct Material
	{
		uint64_t uniqueId = 0;
		std::string name;

		DirectX::XMFLOAT4 Ka = { 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd = { 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::string textureFilenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];

		template<class T>
		void serialize(T& archive) {
			archive(uniqueId, name, Ka, Kd, Ks, textureFilenames);
		}
	};

	std::vector<Mesh> meshes;
	std::unordered_map<uint64_t, Material> materials;
	std::unordered_map<int, Animation> animationClips;

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_Ramp;

public:
	SkinnedMesh(ID3D11Device* device, const char* fbxFilename, bool triangulate = false, float samplingRate = 0);
	virtual ~SkinnedMesh() = default;

	void traverse(FbxNode* fbxNode);
	void fetchMeshes(FbxScene* fbxScene/*, std::vector<Mesh>& meshes*/);
	void fetchMaterials(FbxScene* fbxScene/*, std::unordered_map<uint64_t, Material>& materials*/);
	void fetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose);
	void fetchAnimations(FbxScene* fbxScene, /*std::vector<Animation> animationClips,*/ float samplingRate, int index);
	void createComObjects(ID3D11Device* device, const char* fbxFilename);
	void render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor, const Animation::Keyframe* keyframe = nullptr);
	
	void updateAnimation(Animation::Keyframe& keyframe);
	bool LoadAnimation(const char* animationFilename, float samplingRate, int index);
	void blendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe);

	const std::vector<Mesh>& GetMeshs() const { return meshes; }

protected:
	Scene sceneView;
};

struct BoneInfluence
{
	uint32_t boneIndex;
	float boneWeight;
};

void fetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<std::vector<BoneInfluence>>& boneInfluences);





inline DirectX::XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix) {
	DirectX::XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}
inline DirectX::XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3) {
	DirectX::XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline DirectX::XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4) {
	DirectX::XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}