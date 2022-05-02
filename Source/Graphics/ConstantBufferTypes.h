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