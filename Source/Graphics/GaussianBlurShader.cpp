#include "Misc.h"
#include "GaussianBlurShader.h"
#define _USE_MATH_DEFINES
#include <math.h>

GaussianBlurShader::GaussianBlurShader(ID3D11Device* device)
{

		// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GBShader.InitCSO(device, inputElementDesc, ARRAYSIZE(inputElementDesc),
		"shader\\obj\\GaussianBlurVS.cso", "shader\\obj\\GaussianBlurPS.cso");
	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CBFilter);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, filterConstantBuffer.GetAddressOf());
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
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}
}

// 描画開始
void GaussianBlurShader::Begin(ID3D11DeviceContext* rc)
{
	rc->VSSetShader(GBShader.GetVertexShader().Get(), nullptr, 0);
	rc->PSSetShader(GBShader.GetPixelShader().Get(), nullptr, 0);
	rc->IASetInputLayout(inputLayout.Get());

	rc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	rc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11Buffer* constantBuffers[] =
	{
		filterConstantBuffer.Get(),
	};
	rc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	rc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	rc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON, rc);
	rasterizer->setRasterMode(Rasterizer::RASTER_STATE::SOLID_FALSE, rc);
	rc->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}

// 描画
void GaussianBlurShader::Draw(ID3D11DeviceContext* rc, const SpriteResource* sprite, const GaussianFilterData& gaussianFilterData)
{
	//	ガウスフィルター値の計算
	CBFilter cbFilter;
	CalcGaussianFilter(cbFilter, gaussianFilterData);
	rc->UpdateSubresource(filterConstantBuffer.Get(), 0, 0, &cbFilter, 0, 0);

	UINT stride = sizeof(SpriteResource::Vertex);
	UINT offset = 0;
	rc->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	rc->PSSetShaderResources(0, 1, sprite->GetShaderResourceView().GetAddressOf());
	rc->Draw(4, 0);

}

// 描画終了
void GaussianBlurShader::End(ID3D11DeviceContext* rc)
{
	rc->VSSetShader(nullptr, nullptr, 0);
	rc->PSSetShader(nullptr, nullptr, 0);
	rc->IASetInputLayout(nullptr);
}

void GaussianBlurShader::CalcGaussianFilter(CBFilter& cbFilter, const GaussianFilterData& gaussianFilterData)
{
	int kernelSize = gaussianFilterData.kernelSize;
	// 偶数の場合は奇数に直す
	if (kernelSize % 2 == 0)	kernelSize++;
	kernelSize = max(1, min(MaxKernelSize - 1, kernelSize));
	cbFilter.kernelSize = kernelSize;
	cbFilter.texcel.x = 1.0f / gaussianFilterData.textureSize.x;
	cbFilter.texcel.y = 1.0f / gaussianFilterData.textureSize.y;

	float deviationPow2 = 2.0f * gaussianFilterData.deviation * gaussianFilterData.deviation;
	float sum = 0.0f;
	int id = 0;
	for (int y = -kernelSize / 2; y <= kernelSize / 2; y++)
	{
		for (int x = -kernelSize / 2; x <= kernelSize / 2; x++)
		{

			cbFilter.weights[id].x = (float)x;
			cbFilter.weights[id].y = (float)y;
			//1 / (2πσ ^ 2) 
			float a = 1.0f / (2.0f * M_PI * 50.0f * 50.0f);
			//exp⁡(-(x^2 + y^2) / (2σ^2))をコードに直そう
			float  b = -(x * x + y * y) / (2.0f * 50.0f * 50.0f * 2.0f);
			cbFilter.weights[id].z = a * expf(b);

			sum += cbFilter.weights[id].z;
			id++;
		}
	}
	for (int i = 0; i < cbFilter.kernelSize * cbFilter.kernelSize; i++)
	{
		cbFilter.weights[i].z /= sum;
	}
}
