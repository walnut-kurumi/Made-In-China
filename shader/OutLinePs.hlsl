#include "Outline.hlsli"

Texture2D myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 color = myTexture.Sample(mySampler, pin.texcoord).rgb * pin.color.rgb;
    color = float3(color.rgb - 0.45f);
    smoothstep(0, 1.0f, color.x);
    smoothstep(0, 1.0f, color.y);
    smoothstep(0, 1.0f, color.z);
    return float4(color,pin.color.a);

}