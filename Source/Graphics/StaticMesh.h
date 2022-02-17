#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "Shaders.h"

class StaticMesh
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 materialColor;
	};
	struct Subset
	{
		std::wstring usemtl;
		uint32_t indexStart = 0;
		uint32_t indexCount = 0;
	};
	std::vector<Subset> subsets;
	struct Material
	{
		std::wstring name;
		DirectX::XMFLOAT4 Ka = { 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd = { 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks = { 1.0f, 1.0f, 1.0f, 1.0f };
		std::wstring textureFilenames[2];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[2];
	};
	std::vector<Material> materials;


	DirectX::XMFLOAT3 boundingBox[2] = { {FLT_MAX, FLT_MAX, FLT_MAX }, { -FLT_MAX, -FLT_MAX, -FLT_MAX } };

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	VertexShader vertexShader;
	PixelShader pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

public:
	StaticMesh(ID3D11Device* device, const wchar_t* objFilename, bool inverted = false);
	virtual ~StaticMesh() = default;

	void render(ID3D11DeviceContext* deviceContext,
	const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor);

protected:
	void createComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount,
	uint32_t* indices, size_t indexCount);
};