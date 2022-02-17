#include "FullscreenQuad.hlsli"
#include "Constants.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);

    
float4 main(VS_OUT pin) : SV_TARGET
{    
    float4 color = textureMaps[0].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    color.rgb = smoothstep(0.6, 0.8, dot(color.rgb, float3(1.0f,1.0f,0.7f))) * color.rgb;         
    return float4(color.rgb, alpha);
}