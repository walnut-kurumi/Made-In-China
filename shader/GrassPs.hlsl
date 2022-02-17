#include "Grass.hlsli"
#include "Constants.hlsli"

Texture2D myTexture : register(t0); //�e�N�X�`���[
SamplerState mySampler : register(s0); //�T���v���[

float4 main(PS_IN pin) : SV_TARGET
{
    return lerp(bottomColor, topColor, pin.texcoord.y);
    //return myTexture.Sample(mySampler, pin.texcoord) * pin.color;
}