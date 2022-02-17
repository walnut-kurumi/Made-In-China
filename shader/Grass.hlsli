struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 color : COLOR;
};
typedef VS_OUT GS_IN;

struct GS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

typedef GS_OUT PS_IN;