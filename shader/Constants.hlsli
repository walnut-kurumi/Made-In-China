static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 materialColor;
    row_major float4x4 boneTransforms[MAX_BONES];
};
// cbuffer 定数バッファ
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    row_major float4x4 inverseViewProjection;
    float4 lightDirection;
    float4 cameraPosition;
};

cbuffer BlurCb : register(b4)
{
    float gaussianSigma;
    float bloomIntensity;
    float expo;
};

cbuffer GRASS_CONTANT : register(b2)
{
    float4 bottomColor;
    float4 topColor;
    float bendRotation;
    float widthFactor;
    float heightFactor;
    float grassForward;
    float grassCurve;
    float3 dummy;
}
row_major float4x4 data : register(b3);

cbuffer RADIAL_BLUR_CONSTANT : register(b8)
{
    float TU;
    float TV;
    float BlurPower;
    float demmy1;
};


cbuffer DESTRUCTION_CONSTANT_BUFFER : register(b9)
{
    float _ScaleFactor;
    float _RotationFactor;
    float _PositionFactor;
    float _Destruction;
};

cbuffer RAMP_CONSTANT : register(b10)
{
    float4 ka;
    float4 kd;
    float4 ks;
    float4 ambientColor;
    float4 lightDirectionColor;
};

cbuffer OUTLINE_CONSTANT : register(b11)
{
    float4 normalEdge;
    float4 depthEdge;
};