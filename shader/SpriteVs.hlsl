#include "Sprite.hlsli"
#include "Constants.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    vout.position = position;
    //vout.position = mul(position, data);
    vout.color = color;
    vout.texcoord = texcoord;
    return vout;
}