#pragma once

#include <d3d11.h>
#include "Misc.h"
#include <wrl.h>
class DeppthSteciler
{
public:
	enum DEPTH_STENCIL_STATE
	{
		ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF,
		DSS_END
	};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilStates[DSS_END];
	DeppthSteciler(ID3D11Device* device)
	{
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC DepthStencilDesc{};
		DepthStencilDesc.DepthEnable = TRUE;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&DepthStencilDesc, DepthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE::ZT_ON_ZW_ON)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		DepthStencilDesc.DepthEnable = TRUE;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&DepthStencilDesc, DepthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE::ZT_ON_ZW_OFF)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		DepthStencilDesc.DepthEnable = FALSE;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&DepthStencilDesc, DepthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE::ZT_OFF_ZW_ON)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		DepthStencilDesc.DepthEnable = FALSE;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&DepthStencilDesc, DepthStencilStates[static_cast<size_t>(DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	};
	~DeppthSteciler()
	{
	}
	void setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE Depth, ID3D11DeviceContext* deviceContext)
	{
		deviceContext->OMSetDepthStencilState(DepthStencilStates[Depth].Get(), 0);
	}

};