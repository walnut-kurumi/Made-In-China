 #pragma once

#include<d3d11.h>
#include<wrl.h>
#include<cstdint>
#include"Graphics/Shaders.h"

class fullscreen_quad
{
public:
	fullscreen_quad(ID3D11Device* device);
	virtual ~fullscreen_quad() = default;

private:
	PixelShader embeddedPixelShader;
	VertexShader embeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Shader full;

public:
	void blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView** shadr_resource_view,
		uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);

};