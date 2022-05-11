#pragma once
#pragma once
#include <memory>
#include <wrl.h>
#include "Shader.h"
#include "SpriteResource.h"
#include "DepthStensiler.h"
#include "Rasterizer.h"
#include "ConstantBuffer.h"

class LuminanceExtractionShader
{
public:
	LuminanceExtractionShader(ID3D11Device* device);
	~LuminanceExtractionShader(){}

	void Begin(ID3D11DeviceContext* rc);
	void Draw(ID3D11DeviceContext* rc, const SpriteResource* sprite,const LuminanceExtractionData& lu);
	void End(ID3D11DeviceContext* rc);

private:
	struct CBLuminanceExtraction
	{
		float				threshold;	// 閾値
		float				intensity;	// ブルームの強度
		DirectX::XMFLOAT2	dummy2;
	};

	Shader LECShader;
	std::unique_ptr<DeppthSteciler> depth;
	std::unique_ptr<Rasterizer> rasterizer;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				luminanceExtractionConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
};
