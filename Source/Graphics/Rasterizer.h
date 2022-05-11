#pragma once

#include <d3d11.h>
#include "Misc.h"
#include <wrl.h>
class Rasterizer
{
public:
    enum RASTER_STATE
    {
        SOLID, WIREFRAME, CULL_NONE, 
        WIREFRAME_CULL_NONE, CULL_FRONT,SOLID_FALSE,
        RS_END
    };
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerStates[RS_END];
    Rasterizer(ID3D11Device* device)
    {
        HRESULT hr = S_OK;
		D3D11_RASTERIZER_DESC RasterizerDesc{};
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		// UNIT.21
		//RasterizerDesc.FrontCounterClockwise = FALSE;
		RasterizerDesc.FrontCounterClockwise = TRUE;
		RasterizerDesc.DepthBias = 0;
		RasterizerDesc.DepthBiasClamp = 0;
		RasterizerDesc.SlopeScaledDepthBias = 0;
		RasterizerDesc.DepthClipEnable = TRUE;
		RasterizerDesc.ScissorEnable = FALSE;
		RasterizerDesc.MultisampleEnable = FALSE;
		RasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&RasterizerDesc, RasterizerStates[static_cast<size_t>(RASTER_STATE::SOLID)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		RasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&RasterizerDesc, RasterizerStates[static_cast<size_t>(RASTER_STATE::WIREFRAME)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_NONE;
		RasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&RasterizerDesc, RasterizerStates[static_cast<size_t>(RASTER_STATE::CULL_NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		RasterizerDesc.CullMode = D3D11_CULL_NONE;
		RasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&RasterizerDesc, RasterizerStates[static_cast<size_t>(RASTER_STATE::WIREFRAME_CULL_NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_FRONT;
		RasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&RasterizerDesc, RasterizerStates[static_cast<size_t>(RASTER_STATE::CULL_FRONT)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		RasterizerDesc.FrontCounterClockwise = FALSE;
		RasterizerDesc.DepthBias = 0;
		RasterizerDesc.DepthBiasClamp = 0;
		RasterizerDesc.SlopeScaledDepthBias = 0;
		RasterizerDesc.DepthClipEnable = TRUE;
		RasterizerDesc.ScissorEnable = FALSE;
		RasterizerDesc.MultisampleEnable = FALSE;
		RasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&RasterizerDesc, RasterizerStates[static_cast<size_t>(RASTER_STATE::SOLID_FALSE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    };
    ~Rasterizer()
    {
    }
    void setRasterMode(Rasterizer::RASTER_STATE rasteriz, ID3D11DeviceContext* deviceContext)
    {
        deviceContext->RSSetState(RasterizerStates[rasteriz].Get());
    }

};