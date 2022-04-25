// UNIT.32
#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include"Graphics/Shader.h"

//'fullscreen_quad' dose not have pixel shader and sampler state. you have to make and set pixel shader and sampler state by yourself.
class fullscreen_quad
{
public:
	fullscreen_quad(ID3D11Device *device);
	virtual ~fullscreen_quad() = default;

private:
	VertexShader embedded_vertex_shader;
	PixelShader embedded_pixel_shader;
	Shader FullScreenQ;

public:
	void blit(ID3D11DeviceContext *immediate_contextbool, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);
};
