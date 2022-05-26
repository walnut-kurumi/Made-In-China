#include "Ramp.hlsli"
#include "Constants.hlsli"
#include "RenderingEquation.hlsli"


Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);


float4 main(VS_OUT pin) : SV_TARGET
{                
   
    float4 diffuse2  = { 0.8f, 0.8f, 0.8f, 1.0f };
    float4 specular2 = { 0.8f, 0.8f, 0.8f, 1.0f };    
    float4 ambient = { ambientColor.rgb * ka.rgb, ambientColor.a };
    float4 emissive = { 0.8f, 0.87f, 0.8f, 1.0f };
    
    float4 color = myTexture.Sample(mySampler, pin.texcoord);
    float alpha = color.a;
            
#if 1
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
    const float3 diffuseReflection = pow(color.rgb, GAMMA) * diffuse2.rgb;
#endif
    const float3 specularReflection = specular2.rgb;
    const float shininess = specular2.w;
    const float3 ambientReflection = diffuseReflection * ambient.rgb * ambient.w;
    
    float3 N = normalize(pin.worldNormal.xyz);
    float3 T = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
   
    //float4 normal = myTexture[1].Sample(mySampler[LINEAR], pin.texcoord);
    //normal = (normal * 2.0) - 1.0;
    //N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));

    
    float3 L = normalize(-lightDirection.xyz);
    float3 diffuse = color.rgb * max(0, dot(N, L));
    float3 V = normalize(cameraPosition.xyz - pin.worldPosition.xyz);
    float3 specular = pow(max(0, dot(N, normalize(V + L))), 128 * 0.5f) * 0.1f;
    float3 radiance = diffuse + specular;       
  
        
    float3 irradiance = 0.0;      
    irradiance = lightDirectionColor.rgb * lightDirectionColor.w * max(0, (dot(N, L) * 0.5) + 0.5);
    radiance += RenderingEquation(L, V, N, irradiance, diffuseReflection, specularReflection, shininess);
        
    float3 ambientLight = 1;
    irradiance = ambientLight;
    radiance += irradiance * ambientReflection * float3(1, 1, 0.3) /*ad hoc coefficient*/;
        
  
    float3 halfD = ClacHalfLambert(N, L, lightDirectionColor.rgb, kd.rgb);
    //float3 directionalDiffuse = CalcRampShading(rampMap, mySampler, N, L, lightDirectionColor.rgb, kd.rgb);
    //float3 directionalSpecular = CalcPhongSpecular(N, L, E, lightDirectionColor.rgb, ks.rgb);
                             
    radiance += color.rgb/* * directionalDiffuse*/ + halfD;
		                     
    //return float4(radiance, alpha) * pin.color;    
    //return float4(radiance + directionalDiffuse, alpha) * pin.color;    
    
    return float4(radiance, alpha) * pin.color;
      
}
