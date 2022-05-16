#include "FullscreenQuad.hlsli"

SamplerState samplerStates : register(s0);
Texture2D textureMap : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return textureMap.Sample(samplerStates, pin.texcoord);
}