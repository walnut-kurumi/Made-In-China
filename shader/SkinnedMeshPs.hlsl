#include "SkinnedMesh.hlsli"

Texture2D myTexture[4] : register(t0); //�e�N�X�`���[
SamplerState mySampler[3] : register(s0); //�T���v���[

float4 main(VS_OUT pin) : SV_TARGET
{
    return myTexture[0].Sample(mySampler[2], pin.texcoord) * pin.color;  
}