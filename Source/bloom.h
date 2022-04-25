#pragma once

#include <memory>

#include <iostream>
#include <fstream>
#include <shlwapi.h>
#undef max
#undef min
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>

#include "framebuffer.h"
#include "fullscreen_quad.h"
#include "Graphics/Shader.h"

class bloom : fullscreen_quad
{
public:
	bloom(ID3D11Device* device, uint32_t width, uint32_t height);
	~bloom() = default;
	bloom(const bloom&) = delete;
	bloom& operator =(const bloom&) = delete;
	bloom(bloom&&) noexcept = delete;
	bloom& operator =(bloom&&) noexcept = delete;

	void bloom::make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map);
	void blit(ID3D11DeviceContext* immediate_context);

private:
	std::unique_ptr<framebuffer> glow_extraction;

	static const size_t downsampled_count = 6;
	std::unique_ptr<framebuffer> gaussian_blur[downsampled_count][2];

private:
	PixelShader bloom_extraction_ps;
	PixelShader gaussian_blur_horizontal_ps;
	PixelShader gaussian_blur_vertical_ps;
	PixelShader gaussian_blur_convolution_ps;
	PixelShader gaussian_blur_downsampling_ps;
};
