#include "Framebuffer.h"
#include "Graphics/Misc.h"

Framebuffer::Framebuffer(ID3D11Device* device, uint32_t width, uint32_t height)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer[2];
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


	// �}���`�����_�[�^�[�Q�b�g
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = texture2dDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = device->CreateRenderTargetView(renderTargetBuffer[0].Get(), &renderTargetViewDesc,	renderTargetView[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	hr = device->CreateRenderTargetView(renderTargetBuffer[1].Get(), &renderTargetViewDesc,	renderTargetView[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));



	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(renderTargetBuffer[0].Get(), &shaderResourceViewDesc,	shaderResourceViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	hr = device->CreateShaderResourceView(renderTargetBuffer[1].Get(), &shaderResourceViewDesc, shaderResourceViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));	
		
	

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	hr = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;	
	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));



	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void Framebuffer::Clear(ID3D11DeviceContext* immediateContext,
	float r, float g, float b, float a, float depth)
{
	float color[4]{ r, g, b, a };
	immediateContext->ClearRenderTargetView(renderTargetView->Get(), color);	
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

void Framebuffer::Activate(ID3D11DeviceContext* immediateContext)
{
	
	viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediateContext->RSGetViewports(&viewportCount, cachedViewports);
	immediateContext->OMGetRenderTargets(2, cachedRenderTargetView->ReleaseAndGetAddressOf(),
		cachedDepthStencilView.ReleaseAndGetAddressOf());	

	immediateContext->RSSetViewports(2, &viewport);
	immediateContext->OMSetRenderTargets(2, renderTargetView->GetAddressOf(),
		depthStencilView.Get());	
}

void Framebuffer::Deactivate(ID3D11DeviceContext* immediateContext)
{
	immediateContext->RSSetViewports(viewportCount, cachedViewports);
	immediateContext->OMSetRenderTargets(2, cachedRenderTargetView->GetAddressOf(),
		cachedDepthStencilView.Get());
}