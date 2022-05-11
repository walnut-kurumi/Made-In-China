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

	// ���X�^���C�U�[�X�e�[�g
	{
		// ���C���[�t���[��
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


		// �\���b�h
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

	// �[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC depthstencildesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
		{
			depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		}
		hr = device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		depth = std::make_unique<DeppthSteciler>(device);
	}

	// �T���v���[
	{
		//�T���v���[�X�e�[�g�I�u�W�F�N�g����
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

	//�u�����h
	{
		//�u�����f�B���O�X�e�[�g�I�u�W�F�N�g����
		blender = std::make_unique<Blender>(device);
	}
}

void SkinnedMesh::Begin(ID3D11DeviceContext* dc, Shader shader, bool wire)
{
	// �e�V�F�[�_�[�ݒ�
	dc->VSSetShader(shader.GetVertexShader().Get(), nullptr, 0);
	dc->PSSetShader(shader.GetPixelShader().Get(), nullptr, 0);
	dc->GSSetShader(shader.GetGeometryShader().Get(), nullptr, 0);
	// ���̓��C�A�E�g
	dc->IASetInputLayout(shader.GetInputLayout().Get());

	// �[�x�e�X�g�ݒ�
	//dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
	depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON, dc);

	// �T���v���[�ݒ�
	dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	//blending state object �ݒ�
	blender->setBlendMode(Blender::BLEND_STATE::BS_ALPHA, dc);

	// ���X�^���C�U
	// ���C���[�t���[�����\���b�h��
	wire ? rasterizer->setRasterMode(Rasterizer::RASTER_STATE::WIREFRAME, dc) : rasterizer->setRasterMode(Rasterizer::RASTER_STATE::CULL_NONE, dc);
	//wire ? dc->RSSetState(wireRasterizerState.Get()) : dc->RSSetState(solidRasterizerState.Get());
}

void SkinnedMesh::End(ID3D11DeviceContext* dc)
{
	// �V�F�[�_�̍��Ղ��c���Ȃ�
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
		// �}�l�[�W���[����
		FbxManager* fbxManager = FbxManager::Create();

		// IOSetting�𐶐�
		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);

		//fbxScene����
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

		// �C���|�[�^�[����
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
		bool importStatus = false;
		importStatus = fbxImporter->Initialize(fbxFilename, -1, fbxManager->GetIOSettings());
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		// FbxScene�I�u�W�F�N�g��FBX�t�@�C�����̏��𗬂�����
		importStatus = fbxImporter->Import(fbxScene);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		// �V�[���𗬂����񂾂�Importer�͉��
		fbxImporter->Destroy();

		// �O�p�|���S����
		FbxGeometryConverter fbxConverter(fbxManager);
		if (triangulate) {
			fbxConverter.Triangulate(fbxScene, true, false);
			fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
		}



		//�ċA�őS�m�[�h���擾
		traverse(fbxScene->GetRootNode());

		//���b�V���𒊏o
		fetchMeshes(fbxScene);
		//�}�e���A���𒊏o
		fetchMaterials(fbxScene);
		//�A�j���[�V�����𒊏o
		fetchAnimations(fbxScene, samplingRate, 1341);



		// �}�l�[�W�����:�֘A���邷�ׂẴI�u�W�F�N�g����������
		fbxManager->Destroy();

		//�V���A���쐬
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
		// �}�l�[�W���[����
		FbxManager* fbxManager = FbxManager::Create();

		// IOSetting�𐶐�
		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);

		//fbxScene����
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

		// �C���|�[�^�[����
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
		bool importStatus = false;
		importStatus = fbxImporter->Initialize(animationFilename, -1, fbxManager->GetIOSettings());
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		// FbxScene�I�u�W�F�N�g��FBX�t�@�C�����̏��𗬂�����
		importStatus = fbxImporter->Import(fbxScene);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());


		// �V�[���𗬂����񂾂�Importer�͉��
		fbxImporter->Destroy();


		//�ċA�őS�m�[�h���擾
		traverse(fbxScene->GetRootNode());

		//�A�j���[�V�����𒊏o
		fetchAnimations(fbxScene, samplingRate, index);



		// �}�l�[�W�����:�֘A���邷�ׂẴI�u�W�F�N�g����������
		fbxManager->Destroy();
		//�V���A���쐬
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
		//�ċA����
		traverse(fbxNode->GetChild(childIndex));
	}
}


void SkinnedMesh::render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,const Animation::Keyframe* keyframe) {
	using namespace DirectX;
	// �v���~�e�B�u���[�h�ݒ�
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const Mesh& mesh : meshes) {
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//Constants�̍���
		Constants data{};
		if (keyframe && keyframe->nodes.size() > 0) {

			//************************************************************************************************
			//   "Model ~~ World"
			// 
			//                 "boneTransform"                                  "world"
			//  {(Model ~ bone) * (bone ~ global) * (model ~ global)} ~ {(model ~ global) * (global ~ world)}
			//                                         -1(�t�s��)
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
				//�{�[���̃��[�J���s��ƃO���[�o���s��������i���[���h���W�̈ʒu�Ɂj
				XMStoreFloat4x4(
					&data.boneTransforms[boneIndex],//���f�����猩���{�[���̍s��
					XMLoadFloat4x4(&bone.offsetTransform)//Model ~ bone
					* XMLoadFloat4x4(&boneNode.globalTransform)// bone ~ pose global
					* XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))// model ~ global
				);
			}
		}
		//keyframe���Ȃ��i�A�j���[�V�������Ȃ��j
		else {
			// model ~ world
			XMStoreFloat4x4(
				&data.world,     // Model ~ global             global ~ world
				XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world)
			);
			//�P�ʍs��
			for (size_t boneIndex = 0; boneIndex < MAX_BONES; ++boneIndex) {
				data.boneTransforms[boneIndex] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}

		//�T�u�Z�b�g���Ƃɕ`��
		for (const Subset& subset : mesh.subsets) {
			const Material& material = materials.at(subset.materialUniqueId);
			//materialColor��diffuse������(render�����Ŋe�F�̔Z�W�𒲐�)
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
			? XMMatrixIdentity()												// �e�����Ȃ��F�I�u�W�F�N�g���W�ɒ���
			: XMLoadFloat4x4(&keyframe.nodes.at(parentIndex).globalTransform);	// �e������@�F�e�̍��W���烏�[���h���W�ɕϊ�

		XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
	}
}

//�A�j���[�V�����ǉ�
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
		//���b�V����I��
		if (node.attribute != FbxNodeAttribute::EType::eMesh) continue;
		//�m�[�h�����g��FbxScene����Mesh�������Ă���
		FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
		FbxMesh* fbxMesh = fbxNode->GetMesh();

		//mesh�Z�b�g
		Mesh& mesh = meshes.emplace_back();
		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
		mesh.name = fbxMesh->GetNode()->GetName();
		mesh.nodeIndex = sceneView.indexof(mesh.uniqueId);
		mesh.defaultGlobalTransform = to_xmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());

		std::vector<std::vector<BoneInfluence>> boneInfluences;
		fetchBoneInfluences(fbxMesh, boneInfluences);
		fetchSkeleton(fbxMesh, mesh.bindPose);


		//subset�Z�b�g
		std::vector<Subset>& subsets = mesh.subsets;
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		//subset�̐���material�̐���
		subsets.resize(materialCount > 0 ? materialCount : 1);
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
			//Material�擾
			const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
			subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
		}
		//Material������ꍇ
		if (materialCount > 0) {
			//Material index���擾
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
		
		//���_���|���S�����R(���_)��
		const int polygonCount = fbxMesh->GetPolygonCount();
		mesh.vertices.resize(polygonCount * 3LL);
		mesh.indices.resize(polygonCount * 3LL);

		//uv�Z�b�g
		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);
		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();
		//�|���S���C���f�b�N�X
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
			const int materialIndex =
				materialCount > 0 
				? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex)
				: 0;
			Subset& subset = subsets.at(materialIndex);
			const uint32_t offset = subset.startIndexLocation + subset.indexCount;

			//�|���S���̒��_�i�R�j
			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)	{
				//���_�C���f�b�N�X
				const int vertexIndex = polygonIndex * 3 + positionInPolygon;
				//���_�擾
				Vertex vertex;
				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);
				//�{�[���̉e�����擾
				const std::vector<BoneInfluence>& influencesPerControlPoint = boneInfluences.at(polygonVertex);
				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size();	++influenceIndex) {
					if (influenceIndex < MAX_BONE_INFLUENCES) {
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}
				//�@���擾
				if (fbxMesh->GetElementNormalCount() > 0) {
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				//�@���x�N�g���擾
				if (fbxMesh->GenerateTangentsData(0, false)) {
					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
				}
				//�e�N�X�`��uv�擾
				if (fbxMesh->GetElementUVCount() > 0) {
					FbxVector2 uv;
					bool unmappedUv;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				//���_�C���f�b�N�X�ʂ�ɔz��ɓ���Ă���
				mesh.vertices.at(vertexIndex) = std::move(vertex);
				//mesh.indices.at(vertexIndex) = vertexIndex;
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount++;
			}
		}

		//���E�{�b�N�X�쐬
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
	//�S�Ẵm�[�h�i���b�V���j
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

			//�f�B�t���[�Y
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbxProperty.IsValid()) {
				const FbxDouble3 diffuse = fbxProperty.Get<FbxDouble3>();
				material.Kd.x = static_cast<float>(diffuse[0]);
				material.Kd.y = static_cast<float>(diffuse[1]);
				material.Kd.z = static_cast<float>(diffuse[2]);
				material.Kd.w = 1.0f;
			}

			//�A���r�G���g
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbxProperty.IsValid()) {
				const FbxDouble3 ambient = fbxProperty.Get<FbxDouble3>();
				material.Ka.x = static_cast<float>(ambient[0]);
				material.Ka.y = static_cast<float>(ambient[1]);
				material.Ka.z = static_cast<float>(ambient[2]);
				material.Ka.w = 1.0f;
			}

			//�X�y�L�����[
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbxProperty.IsValid()) {
				const FbxDouble3 specular = fbxProperty.Get<FbxDouble3>();
				material.Ks.x = static_cast<float>(specular[0]);
				material.Ks.y = static_cast<float>(specular[1]);
				material.Ks.z = static_cast<float>(specular[2]);
				material.Ks.w = 1.0f;
			}

			//�e�N�X�`���t�@�C��
			const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
			material.textureFilenames[0] = 
				fbxTexture 
				? fbxTexture->GetRelativeFileName()
				: "";

			//�@���}�b�v�t�@�C��
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
			if (fbxProperty.IsValid()) {
				const FbxFileTexture* fileTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[1] =
					fileTexture 
					? fileTexture->GetRelativeFileName()
					: "";
			}

			//����
			materials.emplace(material.uniqueId, std::move(material));
		}
		//��}�e���A���΍�
		materials.emplace();
	}
}


void SkinnedMesh::fetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose) {
	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		// bone�Ǘ�
		const int clusterCount = skin->GetClusterCount();
		bindPose.bones.resize(clusterCount);
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)	{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);

			Skeleton::Bone& bone = bindPose.bones.at(clusterIndex);
			bone.name = cluster->GetLink()->GetName();
			bone.uniqueId = cluster->GetLink()->GetUniqueID();
			bone.parentIndex = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.nodeIndex = sceneView.indexof(bone.uniqueId);

			//'referenceGlobalInitPosition'��model�imesh�j�̃��[�J����Ԃ���V�[���̃O���[�o���X�y�[�X�ɕϊ����邽�߂Ɏg�p			
			FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);

			// 'clusterGlobalInitPosition'���̃��[�J����Ԃ���V�[���̃O���[�o����Ԃɕϊ����邽�߂Ɏg�p
			FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

			// �s��́A��̎�̃X�L�[�����g�p���Ē�`����܂��BFbxAMatrix ���ϊ���\���ꍇ
			//(�ړ��A��]�A�X�P�[��)�A�s��̍Ō�̍s�́A�ϊ�������\���܂��B
			//	���b�V����Ԃ��獜��ԂɈʒu����ubone.offsetTransform�v�s����쐬���܂��B
			//	���̍s��̓I�t�Z�b�g�s��ƌĂ΂�܂��B
			bone.offsetTransform = to_xmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
		}
	}
}


void SkinnedMesh::fetchAnimations(FbxScene* fbxScene,
	/*std::vector<Animation> animationClips,*/
	float samplingRate, /*���̒l�� 0 �̏ꍇ�A�A�j���[�V���� �f�[�^�͊���̃t���[�� ���[�g�ŃT���v�����O����܂��B*/
	int index) {
#if 0

	FbxArray<FbxString*> animationStackNames;
	//�����̂��ׂẴA�j���[�V���� �X�^�b�N�������
	fbxScene->FillAnimStackNameArray(animationStackNames);
	const int animationStackCount = animationStackNames.GetCount();
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		Animation& animationClip = animationClips.emplace_back();
		animationClip.name = animationStackNames[animationStackIndex]->Buffer();

		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
		fbxScene->SetCurrentAnimationStack(animationStack);

		//�A�j���[�V�����̃^�C�����[�h
		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime oneSecond;

		//�P�b�Ɏg��long long�^�̐��l
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		//1�t���[�����Ԃ�ݒ�
		animationClip.samplingRate =
			samplingRate > 0
			? samplingRate
			: static_cast<float>(oneSecond.GetFrameRate(timeMode));

		//�b�Ԃ̃t���[����
		const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
		//�A�j���[�V�������Ԏ擾
		const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
		const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
		const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();
		for (FbxTime time = startTime; time < stopTime; time += samplingInterval) {
			Animation::Keyframe& keyframe = animationClip.sequence.emplace_back();

			const size_t nodeCount = sceneView.nodes.size();
			keyframe.nodes.resize(nodeCount);
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
				//�X�L���̃N���X�^�[�̃����N������A�X�P���g���m�[�h������
				FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
				if (fbxNode) {
					Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
					// �m�[�h�̕ϊ��s��,�V�[���̃O���[�o�����W�n���w��
					node.globalTransform = to_xmfloat4x4(fbxNode->EvaluateGlobalTransform(time));
					// �e�̃��[�J�����W�n�Ɋւ���m�[�h�̕ϊ��s��
					const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
					node.scaling = to_xmfloat3(localTransform.GetS());
					node.rotation = to_xmfloat4(localTransform.GetQ());
					node.translation = to_xmfloat3(localTransform.GetT());
				}
			}
		}
	}
	//�폜�`
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		delete animationStackNames[animationStackIndex];
	}
#else


	FbxArray<FbxString*> animationStackNames;
	//�����̂��ׂẴA�j���[�V���� �X�^�b�N�������
	fbxScene->FillAnimStackNameArray(animationStackNames);
	const int animationStackCount = animationStackNames.GetCount();
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		Animation animation;
		//Animation& animationClip = animationClips.emplace();
		animation.name = animationStackNames[animationStackIndex]->Buffer();

		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animation.name.c_str());
		fbxScene->SetCurrentAnimationStack(animationStack);

		//�A�j���[�V�����̃^�C�����[�h
		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime oneSecond;

		//�P�b�Ɏg��long long�^�̐��l
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		//1�t���[�����Ԃ�ݒ�
		animation.samplingRate =
			samplingRate > 0
			? samplingRate
			: static_cast<float>(oneSecond.GetFrameRate(timeMode));

		//�b�Ԃ̃t���[����
		const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animation.samplingRate);
		//�A�j���[�V�������Ԏ擾
		const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animation.name.c_str());
		const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
		const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();
		for (FbxTime time = startTime; time < stopTime; time += samplingInterval) {
			Animation::Keyframe& keyframe = animation.sequence.emplace_back();

			const size_t nodeCount = sceneView.nodes.size();
			keyframe.nodes.resize(nodeCount);
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
				//�X�L���̃N���X�^�[�̃����N������A�X�P���g���m�[�h������
				FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
				if (fbxNode) {
					Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
					// �m�[�h�̕ϊ��s��,�V�[���̃O���[�o�����W�n���w��
					node.globalTransform = to_xmfloat4x4(fbxNode->EvaluateGlobalTransform(time));
					// �e�̃��[�J�����W�n�Ɋւ���m�[�h�̕ϊ��s��
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

		// �A�j���[�V�����̒������擾
		animation.secondsLength =( animation.sequence.size() - 1) / animation.samplingRate;
		animationClips.emplace(index, std::move(animation));
	}
	//�폜�`
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

	//�V�F�[�_�[���\�[�X�r���[�I�u�W�F�N�g����
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
	//bone�̉e�����󂯂钸�_�̐�
	boneInfluences.resize(controlPointsCount);

	const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex) {
		//skin�擾
		const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));

		//�e�{�[���ɃA�N�Z�X
		const int clusterCount = fbxSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
			const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);

			//bone���e����^���钸�_�̐�
			const int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex) {
				//bone���e����^���钸�_�ԍ�
				int controlPointIndex = fbxCluster->GetControlPointIndices()[controlPointIndicesIndex];
				//bone�̉e���x
				double controlPointWeight = fbxCluster->GetControlPointWeights()[controlPointIndicesIndex];
				//�擾
				BoneInfluence& boneInfluence = boneInfluences.at(controlPointIndex).emplace_back();
				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
			}
		}
	}
}