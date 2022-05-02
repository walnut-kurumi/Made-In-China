#pragma once
#include <DirectXMath.h>
struct CbScene
{
    DirectX::XMFLOAT4X4	viewProjection;
    DirectX::XMFLOAT4X4 inverseViewProjection;
    DirectX::XMFLOAT4	lightDirection;
    DirectX::XMFLOAT4   cameraPosition;
};

struct Destruction
{
    float scaleFactor;
    float rotationFactor;
    float positionFactor;
    float destruction;
};

struct Grass
{
    DirectX::XMFLOAT4 bottomColor;
    DirectX::XMFLOAT4 topColor;
    float bendRotation;
    float widthFactor;
    float heightFactor;
    float forward;
    float curve;
    DirectX::XMFLOAT3 dummy;
};

struct Outline
{
    DirectX::XMFLOAT4 normalE;
    DirectX::XMFLOAT4 depthE;
};

struct Scroll
{
    DirectX::XMFLOAT2 scroll_direction;
    DirectX::XMFLOAT2 scroll_dummy;
};

struct POST_EFFECT_CONSTANTS
{
    float brightness{ 0.0f };
    float contrast{ 0.058f };
    float hue{ 0.0f };
    float saturation{ 0.078f };

    float bokeh_aperture{ 0.018f };
    float bokeh_focus{ 0.824f };

    float exposure{ 2.771f };
    float post_effect_options{};

    float pantone[4]{ 1.00f, 1.00f, 1.00f, 1.00f }; // The w component is 'intensity'.
};

struct BLOOM_CONSTANTS
{
    float bloom_extraction_threshold = 0.800f;
    float blur_convolution_intensity = 0.500f;

    float bloom_options[2];
};

struct MIST_CONSTANTS
{
    float mist_color[4]{ 0.226f, 0.273f, 0.344f, 1.000f };
    float mist_density[2]{ 0.020f, 0.020f }; // x:extinction, y:inscattering
    float mist_height_falloff[2]{ 1000.000f, 1000.000f }; // x:extinction, y:inscattering
    float height_mist_offset[2]{ 244.300f, 335.505f }; // x:extinction, y:inscattering

    float mist_cutoff_distance{ 0.0f };

    float mist_flow_speed{ 118.123f };
    float mist_flow_noise_scale_factor{ 0.015f };
    float mist_flow_density_lower_limit{ 0.330f };

    float distance_to_sun{ 500.0f };
    float sun_highlight_exponential_factor{ 63.707f }; // Affects the area of influence of the sun's highlights.
    float sun_highlight_intensity{ 1.000f };

    float mist_options[3]{ 0.0f };
};