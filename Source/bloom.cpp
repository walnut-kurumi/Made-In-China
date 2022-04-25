#include "bloom.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Misc.h"

bloom::bloom(ID3D11Device* device, uint32_t width, uint32_t height) : fullscreen_quad(device)
{
	glow_extraction = std::make_unique<framebuffer>(device, width, height, FB_FLAG::COLOR);
	for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
	{
		gaussian_blur[downsampled_index][0] = std::make_unique<framebuffer>(device, width >> downsampled_index, height >> downsampled_index, FB_FLAG::COLOR);
		gaussian_blur[downsampled_index][1] = std::make_unique<framebuffer>(device, width >> downsampled_index, height >> downsampled_index, FB_FLAG::COLOR);
	}
	bloom_extraction_ps.initialize(device, "shader\\obj\\bloom_extraction_ps.cso");
	gaussian_blur_horizontal_ps.initialize(device, "shader\\obj\\gaussian_blur_horizontal_ps.cso");
	gaussian_blur_vertical_ps.initialize(device, "shader\\obj\\gaussian_blur_vertical_ps.cso");
	gaussian_blur_convolution_ps.initialize(device, "shader\\obj\\gaussian_blur_convolution_ps.cso");
	gaussian_blur_downsampling_ps.initialize(device, "shader\\obj\\gaussian_blur_downsampling_ps.cso");
	/*create_ps_from_cso(device, "bloom_extraction_ps.cso", bloom_extraction_ps.GetAddressOf());
	create_ps_from_cso(device, "gaussian_blur_horizontal_ps.cso", gaussian_blur_horizontal_ps.GetAddressOf());
	create_ps_from_cso(device, "gaussian_blur_vertical_ps.cso", gaussian_blur_vertical_ps.GetAddressOf());
	create_ps_from_cso(device, "gaussian_blur_convolution_ps.cso", gaussian_blur_convolution_ps.GetAddressOf());
	create_ps_from_cso(device, "gaussian_blur_downsampling_ps.cso", gaussian_blur_downsampling_ps.GetAddressOf());*/
}

void bloom::make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map)
{
	UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
	immediate_context->RSGetViewports(&viewport_count, cached_viewports);
	immediate_context->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> null_shader_resource_view;

	//Extracting bright color
	glow_extraction->clear(immediate_context, FB_FLAG::COLOR, { 0, 0, 0, 1 });
	glow_extraction->activate(immediate_context);
	fullscreen_quad::blit(immediate_context, &color_map, 0, 1, this->bloom_extraction_ps.getShader());
	glow_extraction->deactivate(immediate_context);

	//Gaussian blur
	//Efficient Gaussian blur with linear sampling
	//http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	// downsampling
	gaussian_blur[0][0]->clear(immediate_context, FB_FLAG::COLOR, { 0, 0, 0, 1 });
	gaussian_blur[0][0]->activate(immediate_context);
	fullscreen_quad::blit(immediate_context, glow_extraction->color_map().GetAddressOf(), 0, 1, this->gaussian_blur_downsampling_ps.getShader());
	gaussian_blur[0][0]->deactivate(immediate_context);

	// ping-pong gaussian blur
	gaussian_blur[0][1]->clear(immediate_context, FB_FLAG::COLOR, { 0, 0, 0, 1 });
	gaussian_blur[0][1]->activate(immediate_context);
	fullscreen_quad::blit(immediate_context, gaussian_blur[0][0]->color_map().GetAddressOf(), 0, 1, this->gaussian_blur_horizontal_ps.getShader());
	gaussian_blur[0][1]->deactivate(immediate_context);

	gaussian_blur[0][0]->clear(immediate_context, FB_FLAG::COLOR, { 0, 0, 0, 1 });
	gaussian_blur[0][0]->activate(immediate_context);
	fullscreen_quad::blit(immediate_context, gaussian_blur[0][1]->color_map().GetAddressOf(), 0, 1, this->gaussian_blur_vertical_ps.getShader());
	gaussian_blur[0][0]->deactivate(immediate_context);

	for (size_t downsampled_index = 1; downsampled_index < downsampled_count; ++downsampled_index)
	{
		// downsampling
		gaussian_blur[downsampled_index][0]->clear(immediate_context, FB_FLAG::COLOR, { 0, 0, 0, 1 });
		gaussian_blur[downsampled_index][0]->activate(immediate_context);
		fullscreen_quad::blit(immediate_context, gaussian_blur[downsampled_index - 1][0]->color_map().GetAddressOf(), 0, 1, this->gaussian_blur_downsampling_ps.getShader());
		gaussian_blur[downsampled_index][0]->deactivate(immediate_context);

		// ping-pong gaussian blur
		gaussian_blur[downsampled_index][1]->clear(immediate_context, FB_FLAG::COLOR, { 0, 0, 0, 1 });
		gaussian_blur[downsampled_index][1]->activate(immediate_context);
		fullscreen_quad::blit(immediate_context, gaussian_blur[downsampled_index][0]->color_map().GetAddressOf(), 0, 1, this->gaussian_blur_horizontal_ps.getShader());
		gaussian_blur[downsampled_index][1]->deactivate(immediate_context);

		gaussian_blur[downsampled_index][0]->clear(immediate_context, FB_FLAG::COLOR, { 0, 0, 0, 1 });
		gaussian_blur[downsampled_index][0]->activate(immediate_context);
		fullscreen_quad::blit(immediate_context, gaussian_blur[downsampled_index][1]->color_map().GetAddressOf(), 0, 1, this->gaussian_blur_vertical_ps.getShader());
		gaussian_blur[downsampled_index][0]->deactivate(immediate_context);
	}

	immediate_context->RSSetViewports(viewport_count, cached_viewports);
	immediate_context->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());
}

void bloom::blit(ID3D11DeviceContext* immediate_context)
{
	UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
	immediate_context->RSGetViewports(&viewport_count, cached_viewports);
	immediate_context->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
	ID3D11ShaderResourceView* cached_shader_resource_views[downsampled_count];
	immediate_context->PSGetShaderResources(0, downsampled_count, cached_shader_resource_views);

	std::vector<ID3D11ShaderResourceView*> shader_resource_views;
	for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
	{
		shader_resource_views.push_back(gaussian_blur[downsampled_index][0]->color_map().Get());
	}
	fullscreen_quad::blit(immediate_context, shader_resource_views.data(), 0, downsampled_count, this->gaussian_blur_convolution_ps.getShader());

	immediate_context->RSSetViewports(viewport_count, cached_viewports);
	immediate_context->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());
	immediate_context->PSSetShaderResources(0, downsampled_count, cached_shader_resource_views);
	for (ID3D11ShaderResourceView* cached_shader_resource_view : cached_shader_resource_views)
	{
		if (cached_shader_resource_view) cached_shader_resource_view->Release();
	}
}

