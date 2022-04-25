// UNIT.32
#include "fullscreen_quad.hlsli"
#include "Constants.hlsli"
#include "noise.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
SamplerComparisonState comparison_sampler_state : register(s5);

#define SCENE 0
#define SCENE_DEPTH 1
Texture2D texture_maps[2] : register(t0);
TextureCube environment_map : register(t4);
Texture2D distortion_texture : register(t5);

// Mist Density : This is the global density factor, which can be thought of as the mist layer's thickness.
// Mist Height Falloff : Height density factor, controls how the density increases as height decreases.Smaller values make the transition larger.
// Height Mist Offset : This controls the height offset of the mist layer relative to the Actor's placed Z (height).
// Mist Cutoff Distance : Scene elements past this distance will not have mist applied.This is useful for excluding skyboxes which already have mist baked into them.
#if 0
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
	
    float mist_options;
};
#endif
float3 mist(float3 fragment_color, float3 mist_color, float3 pixel_coord, float3 eye_coord)
{
    // Using 3d noise to represent the flow of mist.
    const float3 mist_flow_direction = float3(-1.0, -.2, -0.5);
    const float3 mist_flow_coord = pixel_coord.xyz + (mist_flow_direction * mist_flow_speed /* * time * wind_strength*/);
    const float flowing_density = lerp(mist_flow_density_lower_limit, 1.0, noise(fmod(mist_flow_coord * mist_flow_noise_scale_factor, 289)));

    float3 eye_to_pixel = pixel_coord - eye_coord;
	
	//@z is the distance from the eye to the point
    float z = length(pixel_coord - eye_coord);
    z = smoothstep(0, mist_cutoff_distance, z) * z;
	
	// extinction and inscattering coefficients
    const float2 coefficients = mist_density * smoothstep(0.0, mist_height_falloff, height_mist_offset - pixel_coord.y) * flowing_density;

	// extinction and inscattering factors
    const float2 factors = exp(-z * coefficients);

    const float extinction = factors.x;
    const float inscattering = factors.y;
    fragment_color = fragment_color * extinction + mist_color * (1.0 - inscattering);
	
	// Find the sum highlight and use it to blend the mist color
    //const float distance_to_sun = 450;
    float3 sun_position = - /*directional_light_direction[0]*/lightDirection.xyz * 1000;
    float sun_highlight_factor = max(0, dot(normalize(eye_to_pixel), normalize(sun_position - eye_coord)));
    //const float sun_highlight_exponential_factor = 192; // Affects the area of influence of the sun's highlights.
    sun_highlight_factor = pow(sun_highlight_factor, sun_highlight_exponential_factor);

    float3 sky_color = environment_map.Sample(sampler_states[ANISOTROPIC], eye_to_pixel).rgb;
    
    //const float sun_highlight_intensity = 50;
    const float near = 250.0; // The distance at which the effect begins to take effect.
    const float far = distance_to_sun; // The distance at which the effect reaches its maximum value.
    float3 sunhighlight_color = lerp(0, sun_highlight_intensity * (normalize(lightDirection.rgb) + sky_color), sun_highlight_factor * smoothstep(near, far, z));
    //float3 sunhighlight_color = lerp(0, sun_highlight_intensity * directional_light_color[0].rgb, sun_highlight_factor);

    return fragment_color + sunhighlight_color;
}

float3 bokeh_effect(Texture2D color_map, float depth, float2 texcoord, float aperture, float focus, float max_blur)
{
    uint2 dimensions;
    uint mip_level = 0, number_of_samples;
    color_map.GetDimensions(mip_level, dimensions.x, dimensions.y, number_of_samples);

	//Bokeh Effect
	//Max Blur: The maximum amount of blurring.Ranges from 0 to 1
	//Aperture: Bigger values create a shallower depth of field
	//Focus: Controls the focus of the effect
	//Aspect: Controls the blurring effect
	//const float aspect = dimensions.x / dimensions.y;
    const float aspect = dimensions.y / dimensions.x;
    const float2 aspect_correct = float2(1.0, aspect);

	//float4 depth1 = depth_map.Sample(sampler_states[LINEAR], texcoord);
    const float factor = depth - focus;
    const float2 dofblur = clamp(factor * aperture, -max_blur, max_blur);
    const float2 dofblur9 = dofblur * 0.9;
    const float2 dofblur7 = dofblur * 0.7;
    const float2 dofblur4 = dofblur * 0.4;

    float4 color = 0;
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.0, 0.4) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.15, 0.37) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.29, 0.29) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.37, 0.15) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.40, 0.0) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.37, -0.15) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.29, -0.29) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.15, -0.37) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.0, -0.4) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.15, 0.37) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.29, 0.29) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.37, 0.15) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.4, 0.0) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.37, -0.15) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.29, -0.29) * aspect_correct) * dofblur);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.15, -0.37) * aspect_correct) * dofblur);

    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.15, 0.37) * aspect_correct) * dofblur9);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.37, 0.15) * aspect_correct) * dofblur9);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.37, -0.15) * aspect_correct) * dofblur9);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.15, -0.37) * aspect_correct) * dofblur9);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.15, 0.37) * aspect_correct) * dofblur9);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.37, 0.15) * aspect_correct) * dofblur9);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.37, -0.15) * aspect_correct) * dofblur9);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.15, -0.37) * aspect_correct) * dofblur9);

    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.29, 0.29) * aspect_correct) * dofblur7);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.40, 0.0) * aspect_correct) * dofblur7);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.29, -0.29) * aspect_correct) * dofblur7);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.0, -0.4) * aspect_correct) * dofblur7);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.29, 0.29) * aspect_correct) * dofblur7);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.4, 0.0) * aspect_correct) * dofblur7);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.29, -0.29) * aspect_correct) * dofblur7);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.0, 0.4) * aspect_correct) * dofblur7);

    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.29, 0.29) * aspect_correct) * dofblur4);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.4, 0.0) * aspect_correct) * dofblur4);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.29, -0.29) * aspect_correct) * dofblur4);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.0, -0.4) * aspect_correct) * dofblur4);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.29, 0.29) * aspect_correct) * dofblur4);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.4, 0.0) * aspect_correct) * dofblur4);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(-0.29, -0.29) * aspect_correct) * dofblur4);
    color += color_map.Sample(sampler_states[LINEAR_BORDER_BLACK], texcoord + (float2(0.0, 0.4) * aspect_correct) * dofblur4);

    return color.rgb / 41.0;
}
//https://developer.playcanvas.com/en/user-manual/graphics/posteffects/
float3 brightness_contrast(float3 fragment_color, float brightness, float contrast)
{
	//Brightness - Contrast Effect
	//The brightness - contrast effect allows you to modify the brightness and contrast of the rendered image.
	//Brightness: The brighness of the image.Ranges from - 1 to 1 (-1 is solid black, 0 no change, 1 solid white).
	//Contrast : The contrast of the image.Ranges from - 1 to 1 (-1 is solid gray, 0 no change, 1 maximum contrast).
    fragment_color += brightness;
    if (contrast > 0.0)
    {
        fragment_color = (fragment_color - 0.5) / (1.0 - contrast) + 0.5;
    }
    else if (contrast < 0.0)
    {
        fragment_color = (fragment_color - 0.5) * (1.0 + contrast) + 0.5;
    }
    return fragment_color;
}

float3 hue_saturation(float3 fragment_color, float hue, float saturation)
{
	//Hue - Saturation Effect
	//The hue - saturation effect allows you to modify the hue and saturation of the rendered image.
	//Hue: The hue of the image.Ranges from - 1 to 1 (-1 is 180 degrees in the negative direction, 0 no change, 1 is 180 degrees in the postitive direction).
	//Saturation : The saturation of the image.Ranges from - 1 to 1 (-1 is solid gray, 0 no change, 1 maximum saturation).
    float angle = hue * 3.14159265;
    float s = sin(angle), c = cos(angle);
    float3 weights = (float3(2.0 * c, -sqrt(3.0) * s - c, sqrt(3.0) * s - c) + 1.0) / 3.0;
    fragment_color = float3(dot(fragment_color, weights.xyz), dot(fragment_color, weights.zxy), dot(fragment_color, weights.yzx));
    float average = (fragment_color.r + fragment_color.g + fragment_color.b) / 3.0;
    if (saturation > 0.0)
    {
        fragment_color += (average - fragment_color) * (1.0 - 1.0 / (1.001 - saturation));
    }
    else
    {
        fragment_color += (average - fragment_color) * (-saturation);
    }
    return fragment_color;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 sampled_color = texture_maps[SCENE].Sample(sampler_states[POINT], pin.texcoord);
    //return sampled_color;
    float3 fragment_color = sampled_color.rgb;
    float alpha = sampled_color.a;

    float scene_depth = texture_maps[SCENE_DEPTH].Sample(sampler_states[POINT], pin.texcoord).x;
    
    float4 ndc_position;
	//texture space to ndc
    ndc_position.x = pin.texcoord.x * +2 - 1;
    ndc_position.y = pin.texcoord.y * -2 + 1;
    ndc_position.z = scene_depth;
    ndc_position.w = 1;

	//ndc to world space
    float4 world_position = mul(ndc_position, inverseViewProjection);
    world_position = world_position / world_position.w;

	// Apply bokeh effect.
#if 1
    fragment_color = bokeh_effect(texture_maps[SCENE], scene_depth, pin.texcoord, bokeh_aperture, bokeh_focus, 0.1);
#endif

	// Adjusts the hue, saturation, brightness and contrast.
    fragment_color = hue_saturation(fragment_color, hue, saturation);
    fragment_color = brightness_contrast(fragment_color, brightness, contrast);

	// Adapt mist effects.
    fragment_color = mist(fragment_color, mist_color.rgb * lightDirection.rgb * lightDirection.w, world_position.xyz, cameraPosition.xyz);

    return float4(fragment_color, alpha);
}
