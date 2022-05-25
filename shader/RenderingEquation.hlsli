
#define PI 3.141592653

float3 FresnelSchlick(float NoL, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - NoL, 5.0);
}

float3 RenderingEquation(float3 L, float3 V, float3 N, float3 irradiance, float3 diffuseReflection, float3 specularReflection, float shininess)
{

    float3 reflectance = saturate(FresnelSchlick(max(0, dot(N, L)), float3(0.02, 0.02, 0.02)));

    float3 diffuseExitance = diffuseReflection * irradiance * (1.0 - reflectance);
    float3 diffuseRadiance = diffuseExitance / PI;

    float smoothness = shininess;
    float3 specularExitance = specularReflection * irradiance * reflectance;

    float3 H = normalize(L + V);
    float3 specularRadiance = (smoothness + 8) / (8 * PI) * pow(max(0, dot(N, H)), smoothness) * specularExitance;
	
    float3 radiance = diffuseRadiance + specularRadiance;
	
    return radiance;
}

