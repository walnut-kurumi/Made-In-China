#include "Misc.h"
#include "FinalpassShader.h"

FinalpassShader::FinalpassShader(ID3D11Device* device)
{

		// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	FPShader.InitCSO(device, inputElementDesc, ARRAYSIZE(inputElementDesc),
		"shader\\obj\\FinalpassVS.cso", "shader\\obj\\FinalpassPS.cso");

	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CBFinalpass);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, finalpassConstantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}


	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// 深度ステンシルステート
	{
		depth = std::make_unique<DeppthSteciler>(device);
	}

	// ラスタライザーステート
	{
		rasterizer = std::make_unique<Rasterizer>(device);
	}

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}
}

// 描画開始
void FinalpassShader::Begin(ID3D11DeviceContext* rc)
{
	rc->VSSetShader(FPShader.GetVertexShader().Get(), nullptr, 0);
	rc->PSSetShader(FPShader.GetPixelShader().Get(), nullptr, 0);
	rc->IASetInputLayout(inputLayout.Get());

	rc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	rc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	rc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON, rc);
	rasterizer->setRasterMode(Rasterizer::RASTER_STATE::SOLID_FALSE, rc);
	rc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	ID3D11Buffer* constantBuffers[] =
	{
		finalpassConstantBuffer.Get(),
	};
	rc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
}

// 描画
void FinalpassShader::Draw(ID3D11DeviceContext* rc, const SpriteResource* sprite, const ColorGradingData& cogradata, const FinalpassnData& fipasdata)
{
	CBFinalpass cbFinalpass;
	cbFinalpass.hueShift = cogradata.hueShift;
	cbFinalpass.saturation = cogradata.saturation;
	cbFinalpass.brightness = cogradata.brightness;
	//cbFinalpass.hueShift = rc.colorGradingData.hueShift;
	//cbFinalpass.saturation = rc.colorGradingData.saturation;
	//cbFinalpass.brightness = rc.colorGradingData.brightness;
	rc->UpdateSubresource(finalpassConstantBuffer.Get(), 0, 0, &cbFinalpass, 0, 0);

	UINT stride = sizeof(SpriteResource::Vertex);
	UINT offset = 0;
	rc->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	ID3D11ShaderResourceView* srvs[] =
	{
		sprite->GetShaderResourceView().Get(),
		fipasdata.bloomTexture,
		//rc.finalpassnData.bloomTexture,
	};
	rc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
	rc->Draw(4, 0);
}

// 描画終了
void FinalpassShader::End(ID3D11DeviceContext* rc)
{
	rc->VSSetShader(nullptr, nullptr, 0);
	rc->PSSetShader(nullptr, nullptr, 0);
	rc->IASetInputLayout(nullptr);
}
