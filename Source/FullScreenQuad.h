#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include "Graphics/Shaders.h"

class FullScreenQuad
{
public:
	FullScreenQuad(ID3D11Device* device);
	virtual ~FullScreenQuad() = default;

private:
	Shader embeddedShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

public:
	void blit(
		ID3D11DeviceContext* deviceContext,ID3D11ShaderResourceView** shaderResourceView,
		uint32_t startSlot,uint32_t numViews,ID3D11PixelShader* replacedPixelShader = nullptr);

};