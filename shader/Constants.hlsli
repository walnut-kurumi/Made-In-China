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

cbuffer BLOOM_CONSTANTS : register(b6)
{
    float bloom_extraction_threshold;
    float blur_convolution_intensity;

    float2 bloom_options;
};

cbuffer MIST_CONSTANTS : register(b7)
{
    float4 mist_color;
    float2 mist_density; // x:extinction, y:inscattering
    float2 mist_height_falloff; // x:extinction, y:inscattering
    float2 height_mist_offset; // x:extinction, y:inscattering
	
    float mist_cutoff_distance;
	
    float mist_flow_speed;
    float mist_flow_noise_scale_factor;
    float mist_flow_density_lower_limit;
	
    float distance_to_sun;
    float sun_highlight_exponential_factor; // Affects the area of influence of the sun's highlights.
    float sun_highlight_intensity;
	
    float3 mist_options;
};

cbuffer POST_EFFECT_CONSTANTS : register(b8)
{
    float brightness;
    float contrast;
    float hue;
    float saturation;

    float bokeh_aperture;
    float bokeh_focus;

    float exposure;
    float post_effect_options;

    float4 pantone;
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