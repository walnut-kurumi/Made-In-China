#include "SkinnedMesh.hlsli"

Texture2D myTexture[4] : register(t0); //テクスチャー
SamplerState mySampler[3] : register(s0); //サンプラー

float4 main(VS_OUT pin) : SV_TARGET
{
    return myTexture[0].Sample(mySampler[2], pin.texcoord) * pin.color;  
}