#include "Skybox.hlsli"
#include "Constants.hlsli"

SamplerState mySampler : register(s0);

Texture2D skyMap : register(t10);

[earlydepthstencil]
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    //float4 diffuseColor = skyMap.Sample(mySampler[ANISOTROPIC], pin.worldPosition.xyz);
    float4 diffuseColor = skyMap.Sample(mySampler, pin.texcoord);

    const float GAMMA = 2.2;
    float3 diffuseReflection = pow(diffuseColor.rgb, GAMMA) * diffuse.rgb;
    float alpha = diffuseColor.a * diffuse.w;

    const float intensity = 0.8;

    return float4(diffuseReflection.rgb * intensity * lightDirectionColor.rgb * lightDirectionColor.w, alpha);
}