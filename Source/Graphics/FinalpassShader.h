#pragma once

#include <memory>
#include <wrl.h>
#include "Shader.h"
#include "SpriteResource.h"
#include "DepthStensiler.h"
#include "Rasterizer.h"
#include "ConstantBuffer.h"

class FinalpassShader
{
public:
	FinalpassShader(ID3D11Device* device);
	~FinalpassShader() {}

	void Begin(ID3D11DeviceContext* rc);
	void Draw(ID3D11DeviceContext* rc, const SpriteResource* sprite, const ColorGradingData& cogradata, const FinalpassnData& fipasdata);
	void End(ID3D11DeviceContext* rc);

private:
	struct CBFinalpass
	{
		//	�F���␳
		float	hueShift;	// �F������
		float	saturation;	// �ʓx����
		float	brightness;	// ���x����
		float	dummy;
	};

	Shader FPShader;
	std::unique_ptr<DeppthSteciler> depth;
	std::unique_ptr<Rasterizer> rasterizer;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				finalpassConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
};
