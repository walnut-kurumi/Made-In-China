#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "Shaders.h"

class GeometricPrimitive
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 materialColor;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	VertexShader vertexShader;
	PixelShader pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

public:
	GeometricPrimitive(ID3D11Device* device);
	virtual ~GeometricPrimitive() = default;
	void render(ID3D11DeviceContext* deviceContext,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor);

	void createSquare(ID3D11Device* device);

protected:
	void createComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount,
		uint32_t* indices, size_t indexCount);

};