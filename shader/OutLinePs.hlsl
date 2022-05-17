#include "Outline.hlsli"

Texture2D myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = myTexture.Sample(mySampler, pin.texcoord) * pin.color;
    color = float4(color.rgb - 0.45f, 1.0f);
    smoothstep(0, 1.0f, color.x);
    smoothstep(0, 1.0f, color.y);
    smoothstep(0, 1.0f, color.z);
    return color;    

}