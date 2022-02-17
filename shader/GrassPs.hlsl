#include "Grass.hlsli"
#include "Constants.hlsli"

Texture2D myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

float4 main(PS_IN pin) : SV_TARGET
{
    return lerp(bottomColor, topColor, pin.texcoord.y);
    //return myTexture.Sample(mySampler, pin.texcoord) * pin.color;
}