

struct VS_OUT
{
    float4 position : SV_POSITION;     
    float2 texcoord : TEXCOORD;
};
cbuffer ColorCb : register(b2)
{
    float3 rgb;
};