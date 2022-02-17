#include "StaticMesh.h"
#include <fstream>
#include "Texture.h"
#include "Misc.h"
#include <filesystem>

using namespace DirectX;

StaticMesh::StaticMesh(ID3D11Device* device, const wchar_t* objFilename, bool inverted){
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t currentIndex = 0;
	
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texcoords;
	std::vector<std::wstring> mtlFilenames;

	HRESULT hr = S_OK;
	
	//OBJファイルパーサー部
	std::wifstream fin(objFilename);
	_ASSERT_EXPR(fin, L"'OBJ file not found.");
	wchar_t command[256];
	while (fin) {
		fin >> command;
		if (0 == wcscmp(command, L"v")){
			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back({ x, y, z });
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vn")){
			FLOAT i, j, k;
			fin >> i >> j >> k;
			normals.push_back({ i, j, k });
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt")){
			float u, v;
			fin >> u >> v;
			switch (inverted) {
			case false:
				texcoords.push_back({ u, v });
				break;
			case true:
				texcoords.push_back({ u, 1.0f - v });
				break;
			}
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"usemtl")) {
			wchar_t usemtl[MAX_PATH]{};
			fin >> usemtl;
			subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
		}
		else if (0 == wcscmp(command, L"f")){
			for (size_t i = 0; i < 3; i++) {
				Vertex vertex;
				size_t v, vt, vn;
	
				fin >> v;
				vertex.position = positions.at(v - 1);
				if (L'/' == fin.peek())	{
					fin.ignore(1);
					if (L'/' != fin.peek())	{
						fin >> vt;
						vertex.texcoord = texcoords.at(vt - 1);
					}
					if (L'/' == fin.peek())	{
						fin.ignore(1);
						fin >> vn;
						vertex.normal = normals.at(vn - 1);
					}
				}
				vertices.push_back(vertex);
				indices.push_back(currentIndex++);
			}
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"mtllib")){
			wchar_t mtllib[256];
			fin >> mtllib;
			mtlFilenames.push_back(mtllib);
		}
		else {
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();


	auto it = subsets.rbegin();
	it->indexCount = static_cast<uint32_t>(indices.size()) - it->indexStart;
	for (it = subsets.rbegin() + 1; it != subsets.rend(); ++it) {
		it->indexCount = (it - 1)->indexStart - it->indexStart;
	}


	//XTMファイルパーサー部の実装
	std::filesystem::path mtlFilename(objFilename);
	mtlFilename.replace_filename(std::filesystem::path(mtlFilenames[0]).filename());	
	fin.open(mtlFilename);
	//_ASSERT_EXPR(fin, L"'MTL file not found.");
	if (materials.size() == 0){
		for (const Subset& subset : subsets){
			materials.push_back({ subset.usemtl });
		}
	}	
	while (fin) {
		fin >> command;
		if (0 == wcscmp(command, L"map_Kd")){
			fin.ignore();
			wchar_t map_Kd[256];
			fin >> map_Kd;
	
			std::filesystem::path path(objFilename);
			path.replace_filename(std::filesystem::path(map_Kd).filename());
			//textureFilename = path;
			//materials.rbegin()->textureFilename = path;
			materials.rbegin()->textureFilenames[0] = path;
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump")){
			fin.ignore();
			wchar_t map_bump[256];
			fin >> map_bump;
			std::filesystem::path path(objFilename);
			path.replace_filename(std::filesystem::path(map_bump).filename());
			materials.rbegin()->textureFilenames[1] = path;
			fin.ignore(1024, L'\n');			
		}
		else if (0 == wcscmp(command, L"newmtl")){
			fin.ignore();
			wchar_t newmtl[256];
			Material material;
			fin >> newmtl;
			material.name = newmtl;
			materials.push_back(material);
		}
		else if (0 == wcscmp(command, L"Kd")){
			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Kd = { r, g, b, 1 };
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"Ka")) {
			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Ka = { r, g, b, 1 };
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"Ks")) {
			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Ks = { r, g, b, 1 };
			fin.ignore(1024, L'\n');
		}
		else{
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();



	createComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

	// シェーダーオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vertexShader.initialize(device, "shader\\obj\\StaticMeshVs.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	pixelShader.initialize(device, "shader\\obj\\StaticMeshPs.cso");

	//createVsFromCso(device, "shader\\obj\\StaticMeshVs.cso", this->vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	//createPsFromCso(device, "shader\\obj\\StaticMeshPs.cso", this->pixelShader.GetAddressOf());
	
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(Constants);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	}
	hr = device->CreateBuffer(&bufferDesc, nullptr, this->constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	// テクスチャのロード シェーダーリソースビューオブジェクトの生成
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	for (Material& material : materials) {
		if (material.textureFilenames[0].size() > 0) {
			loadTextureFromFile(device, material.textureFilenames[0].c_str(), material.shaderResourceViews[0].GetAddressOf(), &texture2dDesc);
		}
		else {
			makeDummyTexture(device, material.shaderResourceViews[0].GetAddressOf(), 0xFFFFFFFF, 16);
		}
		if (material.textureFilenames[1].size() > 0) {
			loadTextureFromFile(device, material.textureFilenames[1].c_str(), material.shaderResourceViews[1].GetAddressOf(), &texture2dDesc);
		}
		else {
			makeDummyTexture(device, material.shaderResourceViews[1].GetAddressOf(), 0xFFFF7F7F, 16);
		}
	}



	//BoundingBox
	for (const Vertex& v : vertices) {
		boundingBox[0].x = std::min<float>(boundingBox[0].x, v.position.x);
		boundingBox[0].y = std::min<float>(boundingBox[0].y, v.position.y);
		boundingBox[0].z = std::min<float>(boundingBox[0].z, v.position.z);
		boundingBox[1].x = std::max<float>(boundingBox[1].x, v.position.x);
		boundingBox[1].y = std::max<float>(boundingBox[1].y, v.position.y);
		boundingBox[1].z = std::max<float>(boundingBox[1].z, v.position.z);
	}
}

void StaticMesh::render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor){
	uint32_t stride = sizeof(Vertex);
	uint32_t offset = 0;


	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(vertexShader.getInputLayout());

	deviceContext->VSSetShader(vertexShader.getShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.getShader(), nullptr, 0);

	for (const Material& material : materials){
		deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
		deviceContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());

		Constants data = { world, materialColor };
		DirectX::XMStoreFloat4(&data.materialColor, DirectX::XMLoadFloat4(&materialColor) * DirectX::XMLoadFloat4(&material.Kd));
		deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
		deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		for (const auto& subset : subsets){
			if (material.name == subset.usemtl)	{
				deviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);
			}
		}
	}


	//deviceContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	//Constants data = { world, materialColor };
	//deviceContext->UpdateSubresource(this->constantBuffer.Get(), 0, nullptr, &data, 0, 0);
	//deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	//D3D11_BUFFER_DESC bufferDesc{};
	//this->indexBuffer->GetDesc(&bufferDesc);
	//deviceContext->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
}

void StaticMesh::createComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount, uint32_t* indices, size_t indexCount){
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subresourceData{};
	{
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices;
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
	}
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, this->vertexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	{
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresourceData.pSysMem = indices;
	}
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, this->indexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}
