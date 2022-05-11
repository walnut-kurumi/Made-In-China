#include "Misc.h"
#include "SkinnedMesh.h"
#include <sstream>
#include <fstream>
#include <functional>
#include <filesystem>
#include "Texture.h"
#include "Graphics.h"

using namespace DirectX;

void SkinnedMesh::Init(ID3D11Device* device,BOOL frontCounterClockwise)
{
	HRESULT hr = S_OK;

	// ラスタライザーステート
	{
		// ワイヤーフレーム
		{
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_FRONT;
			rasterizerDesc.FrontCounterClockwise = frontCounterClockwise;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			hr = device->CreateRasterizerState(&rasterizerDesc, wireRasterizerState.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}


		// ソリッド
		{
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			rasterizerDesc.FrontCounterClockwise = frontCounterClockwise;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;

			hr = device->CreateRasterizerState(&rasterizerDesc, solidRasterizerState.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
		rasterizer = std::make_unique<Rasterizer>(device);
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC depthstencildesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
		{
			depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		}
		hr = device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		depth = std::make_unique<DeppthSteciler>(device);
	}

	// サンプラー
	{
		//サンプラーステートオブジェクト生成
		CD3D11_SAMPLER_DESC samplerDesc{};
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		}
		hr = device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));		
	}

	//ブレンド
	{
		//ブレンディングステートオブジェクト生成
		blender = std::make_unique<Blender>(device);
	}
}

void SkinnedMesh::Begin(ID3D11DeviceContext* dc, Shader shader, bool wire)
{
	// 各シェーダー設定
	dc->VSSetShader(shader.GetVertexShader().Get(), nullptr, 0);
	dc->PSSetShader(shader.GetPixelShader().Get(), nullptr, 0);
	dc->GSSetShader(shader.GetGeometryShader().Get(), nullptr, 0);
	// 入力レイアウト
	dc->IASetInputLayout(shader.GetInputLayout().Get());

	// 深度テスト設定
	//dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
	depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON, dc);

	// サンプラー設定
	dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	//blending state object 設定
	blender->setBlendMode(Blender::BLEND_STATE::BS_ALPHA, dc);

	// ラスタライザ
	// ワイヤーフレーム化ソリッド化
	wire ? rasterizer->setRasterMode(Rasterizer::RASTER_STATE::WIREFRAME, dc) : rasterizer->setRasterMode(Rasterizer::RASTER_STATE::CULL_NONE, dc);
	//wire ? dc->RSSetState(wireRasterizerState.Get()) : dc->RSSetState(solidRasterizerState.Get());
}

void SkinnedMesh::End(ID3D11DeviceContext* dc)
{
	// シェーダの痕跡を残さない
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
}

SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate) {
	Init(device,false);
	std::filesystem::path cerealFilename(fbxFilename);
	cerealFilename.replace_extension("cereal");
	if (std::filesystem::exists(cerealFilename.c_str())) {
		std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(sceneView, meshes, materials, animationClips);
	}
	else {
		// マネージャー生成
		FbxManager* fbxManager = FbxManager::Create();

		// IOSettingを生成
		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);

		//fbxScene生成
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

		// インポーター生成
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
		bool importStatus = false;
		importStatus = fbxImporter->Initialize(fbxFilename, -1, fbxManager->GetIOSettings());
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		// FbxSceneオブジェクトにFBXファイル内の情報を流し込む
		importStatus = fbxImporter->Import(fbxScene);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		// シーンを流し込んだらImporterは解放
		fbxImporter->Destroy();

		// 三角ポリゴン化
		FbxGeometryConverter fbxConverter(fbxManager);
		if (triangulate) {
			fbxConverter.Triangulate(fbxScene, true, false);
			fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
		}



		//再帰で全ノードを取得
		traverse(fbxScene->GetRootNode());

		//メッシュを抽出
		fetchMeshes(fbxScene);
		//マテリアルを抽出
		fetchMaterials(fbxScene);
		//アニメーションを抽出
		fetchAnimations(fbxScene, samplingRate, 1341);



		// マネージャ解放:関連するすべてのオブジェクトが解放される
		fbxManager->Destroy();

		//シリアル作成
		std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(sceneView, meshes, materials, animationClips);	
	}

	createComObjects(device, fbxFilename);
}

SkinnedMesh::SkinnedMesh(const char* animationFilename, float samplingRate, int index) {
	std::filesystem::path cerealFilename(animationFilename);
	cerealFilename.replace_extension("cereal");
	if (std::filesystem::exists(cerealFilename.c_str())) {
		std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(sceneView, meshes, materials, animationClips);
	}
	else {
		// マネージャー生成
		FbxManager* fbxManager = FbxManager::Create();

		// IOSettingを生成
		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);

		//fbxScene生成
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

		// インポーター生成
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
		bool importStatus = false;
		importStatus = fbxImporter->Initialize(animationFilename, -1, fbxManager->GetIOSettings());
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		// FbxSceneオブジェクトにFBXファイル内の情報を流し込む
		importStatus = fbxImporter->Import(fbxScene);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());


		// シーンを流し込んだらImporterは解放
		fbxImporter->Destroy();


		//再帰で全ノードを取得
		traverse(fbxScene->GetRootNode());

		//アニメーションを抽出
		fetchAnimations(fbxScene, samplingRate, index);



		// マネージャ解放:関連するすべてのオブジェクトが解放される
		fbxManager->Destroy();
		//シリアル作成
		std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(sceneView, meshes, materials, animationClips);
	}
}

void SkinnedMesh::traverse(FbxNode* fbxNode) {
	Scene::Node& node = sceneView.nodes.emplace_back();
	{
		node.attribute =
			fbxNode->GetNodeAttribute()
			? fbxNode->GetNodeAttribute()->GetAttributeType()
			: FbxNodeAttribute::EType::eUnknown;
		node.name = fbxNode->GetName();
		node.uniqueId = fbxNode->GetUniqueID();
		node.parentIndex = sceneView.indexof(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);
	}
	for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex) {
		//再帰部分
		traverse(fbxNode->GetChild(childIndex));
	}
}


void SkinnedMesh::render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,const Animation::Keyframe* keyframe) {
	using namespace DirectX;
	// プリミティブモード設定
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const Mesh& mesh : meshes) {
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//Constantsの合成
		Constants data{};
		if (keyframe && keyframe->nodes.size() > 0) {

			//************************************************************************************************
			//   "Model ~~ World"
			// 
			//                 "boneTransform"                                  "world"
			//  {(Model ~ bone) * (bone ~ global) * (model ~ global)} ~ {(model ~ global) * (global ~ world)}
			//                                         -1(逆行列)
			//************************************************************************************************			
			const Animation::Keyframe::Node& meshNode = keyframe->nodes.at(mesh.nodeIndex);
			XMStoreFloat4x4(
				&data.world, 
				XMLoadFloat4x4(&meshNode.globalTransform) * XMLoadFloat4x4(&world)
			);

			const size_t boneCount = mesh.bindPose.bones.size();
			_ASSERT_EXPR(boneCount < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex) {
				const Skeleton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
			
				const Animation::Keyframe::Node& boneNode = keyframe->nodes.at(bone.nodeIndex);
				//ボーンのローカル行列とグローバル行列を合成（ワールド座標の位置に）
				XMStoreFloat4x4(
					&data.boneTransforms[boneIndex],//モデルから見たボーンの行列
					XMLoadFloat4x4(&bone.offsetTransform)//Model ~ bone
					* XMLoadFloat4x4(&boneNode.globalTransform)// bone ~ pose global
					* XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))// model ~ global
				);
			}
		}
		//keyframeがない（アニメーションがない）
		else {
			// model ~ world
			XMStoreFloat4x4(
				&data.world,     // Model ~ global             global ~ world
				XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world)
			);
			//単位行列
			for (size_t boneIndex = 0; boneIndex < MAX_BONES; ++boneIndex) {
				data.boneTransforms[boneIndex] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}

		//サブセットごとに描画
		for (const Subset& subset : mesh.subsets) {
			const Material& material = materials.at(subset.materialUniqueId);
			//materialColorはdiffuseを入れる(render引数で各色の濃淡を調節)
			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));
			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
			deviceContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());
												
			deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}
	}
}

void SkinnedMesh::updateAnimation(Animation::Keyframe& keyframe) {
	size_t nodeCount = keyframe.nodes.size();
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
		Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
		XMMATRIX S = XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z);
		XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation));
		XMMATRIX T = XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z);

		int64_t parentIndex = sceneView.nodes.at(nodeIndex).parentIndex;
		XMMATRIX P =
			parentIndex < 0 
			? XMMatrixIdentity()												// 親がいない：オブジェクト座標に直接
			: XMLoadFloat4x4(&keyframe.nodes.at(parentIndex).globalTransform);	// 親がいる　：親の座標からワールド座標に変換

		XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
	}
}

//アニメーション追加
bool SkinnedMesh::LoadAnimation(const char* animationFilename, float samplingRate, int index) {
	FbxManager* fbxManager = FbxManager::Create();
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool importStatus = false;
	importStatus = fbxImporter->Initialize(animationFilename);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	fetchAnimations(fbxScene, samplingRate, index);

	fbxManager->Destroy();

	return true;
}

void SkinnedMesh::blendAnimations(const Animation::Keyframe* keyframes[2], float factor,
	Animation::Keyframe& keyframe)
{
	size_t nodeCount = keyframes[0]->nodes.size();
	keyframe.nodes.resize(nodeCount);
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
		XMVECTOR S[2] = {
			XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).scaling),
			XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).scaling)
		};
		XMStoreFloat3(&keyframe.nodes.at(nodeIndex).scaling, XMVectorLerp(S[0], S[1], factor));

		XMVECTOR R[2] = {
			XMLoadFloat4(&keyframes[0]->nodes.at(nodeIndex).rotation),
			XMLoadFloat4(&keyframes[1]->nodes.at(nodeIndex).rotation)
		};
		XMStoreFloat4(&keyframe.nodes.at(nodeIndex).rotation, XMQuaternionSlerp(R[0], R[1], factor));

		XMVECTOR T[2] = {
			XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).translation),
			XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).translation)
		};
		XMStoreFloat3(&keyframe.nodes.at(nodeIndex).translation, XMVectorLerp(T[0], T[1], factor));
	}
}


void SkinnedMesh::fetchMeshes(FbxScene* fbxScene/*, std::vector<Mesh>& meshes*/) {
	for (const Scene::Node& node : sceneView.nodes)	{
		//メッシュを選択
		if (node.attribute != FbxNodeAttribute::EType::eMesh) continue;
		//ノード名を使いFbxSceneからMeshを持ってくる
		FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
		FbxMesh* fbxMesh = fbxNode->GetMesh();

		//meshセット
		Mesh& mesh = meshes.emplace_back();
		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
		mesh.name = fbxMesh->GetNode()->GetName();
		mesh.nodeIndex = sceneView.indexof(mesh.uniqueId);
		mesh.defaultGlobalTransform = to_xmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());

		std::vector<std::vector<BoneInfluence>> boneInfluences;
		fetchBoneInfluences(fbxMesh, boneInfluences);
		fetchSkeleton(fbxMesh, mesh.bindPose);


		//subsetセット
		std::vector<Subset>& subsets = mesh.subsets;
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		//subsetの数＝materialの数に
		subsets.resize(materialCount > 0 ? materialCount : 1);
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
			//Material取得
			const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
			subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
		}
		//Materialがある場合
		if (materialCount > 0) {
			//Material index数取得
			const int polygonCount = fbxMesh->GetPolygonCount();
			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
				const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				subsets.at(materialIndex).indexCount += 3;
			}
			uint32_t offset = 0;
			for (Subset& subset : subsets) {
				subset.startIndexLocation = offset;
				offset += subset.indexCount;
				subset.indexCount = 0;
			}
		}
		
		//頂点をポリゴン＊３(頂点)に
		const int polygonCount = fbxMesh->GetPolygonCount();
		mesh.vertices.resize(polygonCount * 3LL);
		mesh.indices.resize(polygonCount * 3LL);

		//uvセット
		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);
		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();
		//ポリゴンインデックス
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
			const int materialIndex =
				materialCount > 0 
				? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex)
				: 0;
			Subset& subset = subsets.at(materialIndex);
			const uint32_t offset = subset.startIndexLocation + subset.indexCount;

			//ポリゴンの頂点（３つ）
			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)	{
				//頂点インデックス
				const int vertexIndex = polygonIndex * 3 + positionInPolygon;
				//頂点取得
				Vertex vertex;
				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);
				//ボーンの影響を取得
				const std::vector<BoneInfluence>& influencesPerControlPoint = boneInfluences.at(polygonVertex);
				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size();	++influenceIndex) {
					if (influenceIndex < MAX_BONE_INFLUENCES) {
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}
				//法線取得
				if (fbxMesh->GetElementNormalCount() > 0) {
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				//法線ベクトル取得
				if (fbxMesh->GenerateTangentsData(0, false)) {
					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
				}
				//テクスチャuv取得
				if (fbxMesh->GetElementUVCount() > 0) {
					FbxVector2 uv;
					bool unmappedUv;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				//頂点インデックス通りに配列に入れていく
				mesh.vertices.at(vertexIndex) = std::move(vertex);
				//mesh.indices.at(vertexIndex) = vertexIndex;
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount++;
			}
		}

		//境界ボックス作成
		for (const Vertex& v : mesh.vertices) {
			mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
			mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
			mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
			mesh.boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, v.position.x);
			mesh.boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, v.position.y);
			mesh.boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, v.position.z);
		}
	}
}


void SkinnedMesh::fetchMaterials(FbxScene* fbxScene/*, std::unordered_map<uint64_t, Material>& materials*/) {
	//全てのノード（メッシュ）
	const size_t nodeCount = sceneView.nodes.size();
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
		const Scene::Node& node = sceneView.nodes.at(nodeIndex);
		const FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());

		const int materialCount = fbxNode->GetMaterialCount();
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
			const FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);

			Material material;
			material.name = fbxMaterial->GetName();
			material.uniqueId = fbxMaterial->GetUniqueID();
			FbxProperty fbxProperty;

			//ディフューズ
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbxProperty.IsValid()) {
				const FbxDouble3 diffuse = fbxProperty.Get<FbxDouble3>();
				material.Kd.x = static_cast<float>(diffuse[0]);
				material.Kd.y = static_cast<float>(diffuse[1]);
				material.Kd.z = static_cast<float>(diffuse[2]);
				material.Kd.w = 1.0f;
			}

			//アンビエント
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbxProperty.IsValid()) {
				const FbxDouble3 ambient = fbxProperty.Get<FbxDouble3>();
				material.Ka.x = static_cast<float>(ambient[0]);
				material.Ka.y = static_cast<float>(ambient[1]);
				material.Ka.z = static_cast<float>(ambient[2]);
				material.Ka.w = 1.0f;
			}

			//スペキュラー
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbxProperty.IsValid()) {
				const FbxDouble3 specular = fbxProperty.Get<FbxDouble3>();
				material.Ks.x = static_cast<float>(specular[0]);
				material.Ks.y = static_cast<float>(specular[1]);
				material.Ks.z = static_cast<float>(specular[2]);
				material.Ks.w = 1.0f;
			}

			//テクスチャファイル
			const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
			material.textureFilenames[0] = 
				fbxTexture 
				? fbxTexture->GetRelativeFileName()
				: "";

			//法線マップファイル
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
			if (fbxProperty.IsValid()) {
				const FbxFileTexture* fileTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[1] =
					fileTexture 
					? fileTexture->GetRelativeFileName()
					: "";
			}

			//入力
			materials.emplace(material.uniqueId, std::move(material));
		}
		//空マテリアル対策
		materials.emplace();
	}
}


void SkinnedMesh::fetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose) {
	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		// bone管理
		const int clusterCount = skin->GetClusterCount();
		bindPose.bones.resize(clusterCount);
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)	{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);

			Skeleton::Bone& bone = bindPose.bones.at(clusterIndex);
			bone.name = cluster->GetLink()->GetName();
			bone.uniqueId = cluster->GetLink()->GetUniqueID();
			bone.parentIndex = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.nodeIndex = sceneView.indexof(bone.uniqueId);

			//'referenceGlobalInitPosition'はmodel（mesh）のローカル空間からシーンのグローバルスペースに変換するために使用			
			FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);

			// 'clusterGlobalInitPosition'骨のローカル空間からシーンのグローバル空間に変換するために使用
			FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

			// 行列は、列の主体スキームを使用して定義されます。FbxAMatrix が変換を表す場合
			//(移動、回転、スケール)、行列の最後の行は、変換部分を表します。
			//	メッシュ空間から骨空間に位置する「bone.offsetTransform」行列を作成します。
			//	この行列はオフセット行列と呼ばれます。
			bone.offsetTransform = to_xmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
		}
	}
}


void SkinnedMesh::fetchAnimations(FbxScene* fbxScene,
	/*std::vector<Animation> animationClips,*/
	float samplingRate, /*この値が 0 の場合、アニメーション データは既定のフレーム レートでサンプリングされます。*/
	int index) {
#if 0

	FbxArray<FbxString*> animationStackNames;
	//既存のすべてのアニメーション スタック名を入力
	fbxScene->FillAnimStackNameArray(animationStackNames);
	const int animationStackCount = animationStackNames.GetCount();
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		Animation& animationClip = animationClips.emplace_back();
		animationClip.name = animationStackNames[animationStackIndex]->Buffer();

		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
		fbxScene->SetCurrentAnimationStack(animationStack);

		//アニメーションのタイムモード
		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime oneSecond;

		//１秒に使うlong long型の数値
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		//1フレーム時間を設定
		animationClip.samplingRate =
			samplingRate > 0
			? samplingRate
			: static_cast<float>(oneSecond.GetFrameRate(timeMode));

		//秒間のフレーム数
		const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
		//アニメーション時間取得
		const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
		const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
		const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();
		for (FbxTime time = startTime; time < stopTime; time += samplingInterval) {
			Animation::Keyframe& keyframe = animationClip.sequence.emplace_back();

			const size_t nodeCount = sceneView.nodes.size();
			keyframe.nodes.resize(nodeCount);
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
				//スキンのクラスターのリンク名から、スケルトンノードを検索
				FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
				if (fbxNode) {
					Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
					// ノードの変換行列,シーンのグローバル座標系を指定
					node.globalTransform = to_xmfloat4x4(fbxNode->EvaluateGlobalTransform(time));
					// 親のローカル座標系に関するノードの変換行列
					const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
					node.scaling = to_xmfloat3(localTransform.GetS());
					node.rotation = to_xmfloat4(localTransform.GetQ());
					node.translation = to_xmfloat3(localTransform.GetT());
				}
			}
		}
	}
	//削除～
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		delete animationStackNames[animationStackIndex];
	}
#else


	FbxArray<FbxString*> animationStackNames;
	//既存のすべてのアニメーション スタック名を入力
	fbxScene->FillAnimStackNameArray(animationStackNames);
	const int animationStackCount = animationStackNames.GetCount();
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		Animation animation;
		//Animation& animationClip = animationClips.emplace();
		animation.name = animationStackNames[animationStackIndex]->Buffer();

		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animation.name.c_str());
		fbxScene->SetCurrentAnimationStack(animationStack);

		//アニメーションのタイムモード
		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime oneSecond;

		//１秒に使うlong long型の数値
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		//1フレーム時間を設定
		animation.samplingRate =
			samplingRate > 0
			? samplingRate
			: static_cast<float>(oneSecond.GetFrameRate(timeMode));

		//秒間のフレーム数
		const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animation.samplingRate);
		//アニメーション時間取得
		const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animation.name.c_str());
		const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
		const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();
		for (FbxTime time = startTime; time < stopTime; time += samplingInterval) {
			Animation::Keyframe& keyframe = animation.sequence.emplace_back();

			const size_t nodeCount = sceneView.nodes.size();
			keyframe.nodes.resize(nodeCount);
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
				//スキンのクラスターのリンク名から、スケルトンノードを検索
				FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
				if (fbxNode) {
					Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
					// ノードの変換行列,シーンのグローバル座標系を指定
					node.globalTransform = to_xmfloat4x4(fbxNode->EvaluateGlobalTransform(time));
					// 親のローカル座標系に関するノードの変換行列
					const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
					node.scaling = to_xmfloat3(localTransform.GetS());
					node.rotation = to_xmfloat4(localTransform.GetQ());
					node.translation = to_xmfloat3(localTransform.GetT());
					int hour, minute, second, frame, field, residual;
					time.GetTime(hour, minute, second, frame, field, residual, FbxTime::eFrames1000);
					keyframe.seconds = frame / 1000.0f;
				}
			}
		}

		// アニメーションの長さを取得
		animation.secondsLength =( animation.sequence.size() - 1) / animation.samplingRate;
		animationClips.emplace(index, std::move(animation));
	}
	//削除～
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		delete animationStackNames[animationStackIndex];
	}

#endif
}


void SkinnedMesh::createComObjects(ID3D11Device* device, const char* fbxFilename) {
	HRESULT hr = S_OK;
	for (Mesh& mesh : meshes) {
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subresourceData{};
		{
			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;

			subresourceData.pSysMem = mesh.vertices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;
		}
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		{
			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			subresourceData.pSysMem = mesh.indices.data();
		}
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	//シェーダーリソースビューオブジェクト生成
	for (std::unordered_map<uint64_t, Material>::iterator it = materials.begin(); it != materials.end(); ++it) {
		for (size_t textureIndex = 0; textureIndex < 2; ++textureIndex) {
			if (it->second.textureFilenames[textureIndex].size() > 0) {
				std::filesystem::path path(fbxFilename);
				path.replace_filename(it->second.textureFilenames[textureIndex]);
				D3D11_TEXTURE2D_DESC texture2dDesc;
				loadTextureFromFile(device, path.c_str(),
					it->second.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc);
			}
			else {
				makeDummyTexture(device,
					it->second.shaderResourceViews[textureIndex].GetAddressOf(),
					textureIndex == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16
				);
			}
		}
	}

	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(Constants);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	}
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void fetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<std::vector<BoneInfluence>>& boneInfluences) {
	const int controlPointsCount = fbxMesh->GetControlPointsCount();
	//boneの影響を受ける頂点の数
	boneInfluences.resize(controlPointsCount);

	const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex) {
		//skin取得
		const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));

		//各ボーンにアクセス
		const int clusterCount = fbxSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
			const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);

			//boneが影響を与える頂点の数
			const int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex) {
				//boneが影響を与える頂点番号
				int controlPointIndex = fbxCluster->GetControlPointIndices()[controlPointIndicesIndex];
				//boneの影響度
				double controlPointWeight = fbxCluster->GetControlPointWeights()[controlPointIndicesIndex];
				//取得
				BoneInfluence& boneInfluence = boneInfluences.at(controlPointIndex).emplace_back();
				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
			}
		}
	}
}