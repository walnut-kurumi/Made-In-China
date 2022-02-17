
#define PI 3.141592653

float3 FresnelSchlick(float NoL, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - NoL, 5.0);
}

float3 RenderingEquation(float3 L, float3 V, float3 N, float3 irradiance, float3 diffuseReflection, float3 specularReflection, float shininess)
{
#if 0
	float tones = 4;
	//irradiance = ceil(irradiance * tones) / tones;
	irradiance = floor(irradiance * tones) / tones;
#endif
	
#if 0
	float3 reflectance = 0.2;
#else
    float3 reflectance = saturate(FresnelSchlick(max(0, dot(N, L)), float3(0.02, 0.02, 0.02)));
#endif

    float3 diffuseExitance = diffuseReflection * irradiance * (1.0 - reflectance);
    float3 diffuseRadiance = diffuseExitance / PI;

    float smoothness = shininess;
    float3 specularExitance = specularReflection * irradiance * reflectance;
#if 0
	float3 R = reflect(-L, N);
	float3 specular_radiance = pow(max(0, dot(V, R)), smoothness) * specular_exitance;
#else
    float3 H = normalize(L + V);
    float3 specularRadiance = (smoothness + 8) / (8 * PI) * pow(max(0, dot(N, H)), smoothness) * specularExitance;
#endif
	
    float3 radiance = diffuseRadiance + specularRadiance;
	
    return radiance;
}

