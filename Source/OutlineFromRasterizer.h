#pragma once
#include "Graphics/Graphics.h"

class RenderStateKARI
{
	// •`‰æÝ’è
public:
	enum CullType
	{
		CU_FRONT,    // 0:•\–Ê•`‰æ
		CU_BACK,	 // 1:— –Ê•`‰æ
		CU_NONE,	 // 2:—¼–Ê•`‰æ
		CU_END,
	};
	static void InitializeRasterizer() {
		ID3D11Device* device = Graphics::Ins().GetDevice();
		D3D11_RASTERIZER_DESC rasterizerDesc{};
		{
			rasterizerDesc.FillMode = /*D3D11_FILL_SOLID; */D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_FRONT;
			rasterizerDesc.FrontCounterClockwise = true;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = true;
			rasterizerDesc.ScissorEnable = false;
			rasterizerDesc.MultisampleEnable = false;
			rasterizerDesc.AntialiasedLineEnable = false;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
		}
		device->CreateRasterizerState(&rasterizerDesc, rasterizer[CU_FRONT].GetAddressOf());

		//•\–ÊƒJƒŠƒ“ƒO
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		device->CreateRasterizerState(&rasterizerDesc, rasterizer[CU_BACK].GetAddressOf());

		//ƒJƒŠƒ“ƒO–³‚µ = —¼–Ê•`‰æ
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		device->CreateRasterizerState(&rasterizerDesc, rasterizer[CU_NONE].GetAddressOf());
	}
	static void SetCullMode(int type) {
		Graphics::Ins().GetDeviceContext()->RSSetState(rasterizer[type].Get());
	}
private:
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer[CU_END];
};

