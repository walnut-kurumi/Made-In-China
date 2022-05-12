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

struct scene_col
{
    DirectX::XMFLOAT3 rgb;
    float dummy;
};

struct scene_blur
{
    float sigma;
    float intensity;
    float dummy0;
    float dummy1;
};

struct Radial_Blur
{
    float TU;
    float TV;
    float BlurPower;
    float dummy;
};

// 色調補正情報
struct ColorGradingData
{
    float	hueShift = 0;	// 色相調整
    float	saturation = 1;	// 彩度調整
    float	brightness = 1;	// 明度調整
    float	dummy;
};

// ガウスフィルター計算情報
struct GaussianFilterData
{
    int					kernelSize = 8;		// カーネルサイズ
    float				deviation = 10.0f;	// 標準偏差
    DirectX::XMFLOAT2	textureSize;			// 暈すテクスチャのサイズ
};

// 高輝度抽出用情報
struct LuminanceExtractionData
{
    float				threshold = 0.5f;	// 閾値
    float				intensity = 1.0f;	// ブルームの強度
    DirectX::XMFLOAT2	dummy2;
};

// ポストエフェクトの最終パス用情報
struct FinalpassnData
{
    //	ブルームテクスチャ
    ID3D11ShaderResourceView* bloomTexture;
};