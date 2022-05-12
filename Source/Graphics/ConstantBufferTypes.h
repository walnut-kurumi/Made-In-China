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

// �F���␳���
struct ColorGradingData
{
    float	hueShift = 0;	// �F������
    float	saturation = 1;	// �ʓx����
    float	brightness = 1;	// ���x����
    float	dummy;
};

// �K�E�X�t�B���^�[�v�Z���
struct GaussianFilterData
{
    int					kernelSize = 8;		// �J�[�l���T�C�Y
    float				deviation = 10.0f;	// �W���΍�
    DirectX::XMFLOAT2	textureSize;			// �򂷃e�N�X�`���̃T�C�Y
};

// ���P�x���o�p���
struct LuminanceExtractionData
{
    float				threshold = 0.5f;	// 臒l
    float				intensity = 1.0f;	// �u���[���̋��x
    DirectX::XMFLOAT2	dummy2;
};

// �|�X�g�G�t�F�N�g�̍ŏI�p�X�p���
struct FinalpassnData
{
    //	�u���[���e�N�X�`��
    ID3D11ShaderResourceView* bloomTexture;
};