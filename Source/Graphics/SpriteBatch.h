#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include "Shaders.h"

class SpriteBatch
{
public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	                              
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	const size_t maxVertices;
	std::vector<Vertex> vertices;


	void render(ID3D11DeviceContext* deviceContext,
		float dx, float dy, float dw, float dh);//短径の左上　　　　　サイズ
	void render(ID3D11DeviceContext* deviceContext,
		float dx, float dy, float dw, float dh,//短径の左上　　　　　サイズ
		float r, float g, float b, float a,//カラー
		float angle /*Degree*/);//回転
	void render(ID3D11DeviceContext* deviceContext,
		float dx, float dy, float dw, float dh,//短径の左上　　　　　サイズ
		float r, float g, float b, float a,//カラー
		float angle /*Degree*/,//回転
		float sx, float sy, float sw, float sh);
	SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites);
	~SpriteBatch();


	void begin(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* shaderResourceView);
	void begin(ID3D11DeviceContext* deviceContext);
	void end(ID3D11DeviceContext* deviceContext);


	inline void rotate(float& x, float& y, float cx, float cy, float cos, float sin) {
		x -= cx;
		y -= cy;
		float tx = x, ty = y;
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;
		x += cx;
		y += cy;
	}
};