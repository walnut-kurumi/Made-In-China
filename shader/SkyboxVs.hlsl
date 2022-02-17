#include "Skybox.hlsli"
#include "Constants.hlsli"


VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
   
    float4 blendedPosition = { 0, 0, 0, 1 };
    float4 blendedNormal = { 0, 0, 0, 0 };
    float4 blendedTangent = { 0, 0, 0, 0 };
    for (int boneIndex = 0; boneIndex < 4; ++boneIndex)
    {
        blendedPosition += vin.bone_weights[boneIndex]
        * mul(vin.position, boneTransforms[vin.bone_indices[boneIndex]]);
        blendedNormal += vin.bone_weights[boneIndex]
        * mul(vin.normal, boneTransforms[vin.bone_indices[boneIndex]]);
        blendedTangent += vin.bone_weights[boneIndex]
        * mul(vin.tangent, boneTransforms[vin.bone_indices[boneIndex]]);
    }
    vin.position = float4(blendedPosition.xyz, 1.0f);
    vin.normal = float4(blendedNormal.xyz, 0.0f);
    vin.tangent = float4(blendedTangent.xyz, 0.0f);
  
          
    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, viewProjection));
    
    vout.worldPosition = mul(vin.position, world);
    vin.normal.w = 0;
    vout.worldNormal = normalize(mul(vin.normal, world));
    vout.worldTangent = normalize(mul(vin.tangent, world));
    vout.worldTangent.w = sigma;
    vout.texcoord = vin.texcoord;
    
    vout.color = materialColor;

    return vout;
}
