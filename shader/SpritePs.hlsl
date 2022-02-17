#include "Sprite.hlsli"

Texture2D diffuseMap : register(t0); // ‰æ‘œ
SamplerState diffuseMapSamplerState : register(s0);

float4 main(PS_IN pin) : SV_TARGET0
{
    return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
}