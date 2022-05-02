// UNIT.32
#include "fullscreen_quad.h"
#include "Graphics/Misc.h"

fullscreen_quad::fullscreen_quad(ID3D11Device *device)
{

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	embedded_vertex_shader.initialize(device, "shader\\obj\\fullscreen_quad_vs.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	embedded_pixel_shader.initialize(device, "shader\\obj\\fullscreen_quad_ps.cso");
	/*create_vs_from_cso(device, "fullscreen_quad_vs.cso", embedded_vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "fullscreen_quad_ps.cso", embedded_pixel_shader.ReleaseAndGetAddressOf());*/
}
void fullscreen_quad::blit(ID3D11DeviceContext *immediate_context, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
	// UNIT.99
	ID3D11ShaderResourceView* cached_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediate_context->PSGetShaderResources(start_slot, num_views, cached_shader_resource_views);

	immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

	immediate_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(nullptr);

	immediate_context->VSSetShader(this->embedded_vertex_shader.getShader(), 0, 0);
	replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) : immediate_context->PSSetShader(this->embedded_pixel_shader.getShader(), 0, 0);
	immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

	immediate_context->Draw(4, 0);

	// UNIT.99
	immediate_context->PSSetShaderResources(start_slot, num_views, cached_shader_resource_views);
	for (ID3D11ShaderResourceView* cached_shader_resource_view : cached_shader_resource_views)
	{
		if (cached_shader_resource_view) cached_shader_resource_view->Release();
	}
}
