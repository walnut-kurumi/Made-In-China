// UNIT.99
#include "Constants.hlsli"

Texture2D hdr_color_buffer_texture : register(t0);
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float4 sampled_color = hdr_color_buffer_texture.Sample(sampler_states[POINT], texcoord);
    return float4(smoothstep(bloom_extraction_threshold, bloom_extraction_threshold + 0.5, dot(sampled_color.rgb, float3(0.299, 0.587, 0.114))) * sampled_color.rgb, sampled_color.a);
}
