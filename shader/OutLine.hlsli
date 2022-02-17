struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

typedef VS_OUT GS_IN;
typedef GS_IN GS_OUT;
typedef GS_OUT PS_IN;
