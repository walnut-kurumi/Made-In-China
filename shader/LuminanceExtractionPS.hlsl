#include "FullscreenQuad.hlsli"
#include "Constants.hlsli"

SamplerState samplerStates : register(s0);
Texture2D textureMaps[4] : register(t0);

    
float4 main(VS_OUT pin) : SV_TARGET
{    
    float4 color = textureMaps[0].Sample(samplerStates, pin.texcoord);
    float alpha = color.a;
    color.rgb = smoothstep(0.6, 0.8, dot(color.rgb, float3(rgb.x, rgb.y, rgb.z))) * color.rgb;         
    return float4(color.rgb, alpha);
}