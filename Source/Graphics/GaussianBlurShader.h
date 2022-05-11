#pragma once
#include <memory>
#include <wrl.h>
#include "Shader.h"
#include "SpriteResource.h"
#include "ConstantBuffer.h"
#include "DepthStensiler.h"
#include "Rasterizer.h"

class GaussianBlurShader
{
	static const int MaxKernelSize = 16;
	struct CBFilter
	{
		DirectX::XMFLOAT4	weights[MaxKernelSize * MaxKernelSize];
		float	kernelSize;
		DirectX::XMFLOAT2	texcel;
		float	dummy;
	};

public:
	GaussianBlurShader(ID3D11Device* device);
	~GaussianBlurShader(){}

	void Begin(ID3D11DeviceContext* rc);
	void Draw(ID3D11DeviceContext* rc, const SpriteResource* sprite, const GaussianFilterData& gaussianFilterData);
	void End(ID3D11DeviceContext* rc);

private:
	// フィルター値計算
	void CalcGaussianFilter(CBFilter& cbFilter, const GaussianFilterData& gaussianFilterData);

private:
	Shader GBShader;
	std::unique_ptr<DeppthSteciler> depth;
	std::unique_ptr<Rasterizer> rasterizer;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				filterConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
};
