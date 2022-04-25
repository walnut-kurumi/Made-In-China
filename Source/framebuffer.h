#pragma once

// UNIT.32
#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

#include <directxmath.h>
#include "Graphics/Shader.h"

// UNIT.99
enum class FB_FLAG : uint8_t
{
	COLOR = 0x01,
	DEPTH = 0x02,
	STENCIL = 0x04,
	COLOR_DEPTH_STENCIL = COLOR | DEPTH | STENCIL,
	COLOR_DEPTH = COLOR | DEPTH,
	DEPTH_STENCIL = DEPTH | STENCIL,
};

class framebuffer
{
public:
	framebuffer(ID3D11Device* device, uint32_t width, uint32_t height, FB_FLAG flags = FB_FLAG::COLOR_DEPTH_STENCIL/*UNIT.99*/, bool enable_msaa = false/*UNIT.99*/, int subsamples = 1/*UNIT.99*/, bool generate_mips = false/*UNIT.99*/);
	virtual ~framebuffer() = default;


	void clear(ID3D11DeviceContext* immediate_context, FB_FLAG flags = FB_FLAG::COLOR_DEPTH_STENCIL/*UNIT.99*/, DirectX::XMFLOAT4 color = { 0, 0, 0, 1 }, float depth = 1, uint8_t stencil = 0/*UNIT.99*/);

	void activate(ID3D11DeviceContext* immediate_context, FB_FLAG flags = FB_FLAG::COLOR_DEPTH_STENCIL/*UNIT.99*/);
	void deactivate(ID3D11DeviceContext* immediate_context);

	// UNIT.99
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& color_map() { return shader_resource_views[0]; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& depth_map() { return shader_resource_views[1]; }

	// UNIT.99
	void generate_mips(ID3D11DeviceContext* immediate_context);
	void resolve(ID3D11DeviceContext* immediate_context, framebuffer* destination);

protected:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];
	D3D11_VIEWPORT viewport;

private:
	UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;

	// UNIT.99
	VertexShader embedded_vertex_shader;
	PixelShader embedded_pixel_shader;
};
