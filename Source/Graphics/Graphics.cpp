#include "Graphics.h"
#include "Shaders.h"
#include "Misc.h"
#include "Texture.h"

Graphics* Graphics::instance{};

Graphics::Graphics(HWND hwnd) {
	// インスタンス設定
	_ASSERT_EXPR(instance == nullptr, "already instantiated");
	instance = this;


	if (!InitializeDirectX(hwnd)) return;
	if (!InitializeShaders()) return;
	if (!InitializeScene()) return;

}


bool Graphics::InitializeDirectX(HWND hwnd) {
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

	// 画面のサイズを取得する。
	// 設定時、UINTで使いたいため
	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT windowWidth = rc.right - rc.left;
	UINT windowHeight = rc.bottom - rc.top;

	this->windowWidth = static_cast<float>(windowWidth);
	this->windowHeight = static_cast<float>(windowHeight);



	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	{
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = windowWidth;
		swapChainDesc.BufferDesc.Height = windowHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
	}
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		&featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		this->swapChain.GetAddressOf(),
		this->device.GetAddressOf(),
		NULL,
		this->deviceContext.GetAddressOf()
	);
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


	//レンダーターゲットビューの作成
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));	

	//深度ステンシルビューの設定
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	{
		texture2dDesc.Width = windowWidth;
		texture2dDesc.Height = windowHeight;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
	}
	hr = device->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));




	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	{
		depthStencilViewDesc.Format = texture2dDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
	}
	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, this->depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//Viewport作成
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
	this->deviceContext->RSSetViewports(1, &viewport);

	return true;
}

bool Graphics::InitializeShaders() {

	Shaders::Ins()->Init(this->device.Get());
	return true;
}

bool Graphics::InitializeScene()
{


	return true;
}
