#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include <Memory>
#include "Shaders.h"
#include "Blender.h"

class Sprite
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

	void render(ID3D11DeviceContext* deviceContext,
		float dx, float dy, float dw, float dh, //短径の左上　　　　　サイズ
		Shader shader);
	void render(ID3D11DeviceContext* deviceContext,
		float dx, float dy, float dw, float dh,//短径の左上　　　　　サイズ
		float r, float g, float b, float a,//カラー
		float angle, /*Degree*/ //回転
		Shader shader);
	void render(ID3D11DeviceContext* deviceContext,
		float dx, float dy, float dw, float dh,//短径の左上　　　　　サイズ
		float r, float g, float b, float a,//カラー
		float angle /*Degree*/,//回転
		float sx, float sy, float sw, float sh,
		Shader shader);

	Sprite(ID3D11Device* device, const wchar_t* filename);
	Sprite(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView);
	~Sprite();


	void Sprite::textout(ID3D11DeviceContext* deviceContext, std::string s,
		float x, float y, float w, float h, float r, float g, float b, float a);


	inline void rotate(float& x, float& y, float cx, float cy, float cos, float sin) {
		x -= cx;
		y -= cy;
		float tx = x, ty = y;
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;
		x += cx;
		y += cy;
	}

private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::unique_ptr<Blender> blender;
	
	bool isLoadFile = true;


};