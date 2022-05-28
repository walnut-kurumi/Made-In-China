#include "SkinnedMesh.hlsli"

//Texture2D myTexture[4] : register(t0); //テクスチャー
//SamplerState mySampler : register(s0); //サンプラー

float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(1, 1, 0, 1);
    //return myTexture[0].Sample(mySampler, pin.texcoord) * pin.color;
}