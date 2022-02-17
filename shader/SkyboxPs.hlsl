#include "Skybox.hlsli"
#include "Constants.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState mySampler[3] : register(s0);

Texture2D skyMap : register(t10);

[earlydepthstencil]
PS_OUT main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    //float4 diffuseColor = skyMap.Sample(mySampler[ANISOTROPIC], pin.worldPosition.xyz);
    float4 diffuseColor = skyMap.Sample(mySampler[ANISOTROPIC], pin.texcoord);

    const float GAMMA = 2.2;
    float3 diffuseReflection = pow(diffuseColor.rgb, GAMMA) * diffuse.rgb;
    float alpha = diffuseColor.a * diffuse.w;

    const float intensity = 0.8;

    PS_OUT pout;
    pout.color0 = float4(diffuseReflection.rgb * intensity * lightDirectionColor.rgb * lightDirectionColor.w, alpha);
    //pout.color1 = float4(diffuseReflection.rgb * intensity * lightDirectionColor.rgb * lightDirectionColor.w, alpha);
    pout.color1 = float4(pin.worldNormal.rgb * 0.5f, 1.0f);
    return pout;
}