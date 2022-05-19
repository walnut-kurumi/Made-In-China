struct VS_OUT
{
    float4 position : SV_POSITION;
};

cbuffer TrailCb : register(b0)
{
    row_major float4x4 vp;
};