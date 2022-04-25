

struct VS_OUT
{
    float4 position : SV_POSITION;     
    float2 texcoord : TEXCOORD;
};
cbuffer ColorCb : register(b2)
{
    float4 rgb;
};
cbuffer BlurCb : register(b3)
{    
    float gaussianSigma;
    float bloomIntensity;
    //float exposure;
};