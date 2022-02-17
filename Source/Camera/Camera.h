#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Graphics/Vec.h"
using namespace DirectX;

class Camera
{
public:
	//�p�[�X�y�N�e�B�u�ݒ�
	void SetPerspectiveFov(float fov, float aspect, float nearZ, float farZ);
	//���_�E�����_�ݒ�
	void SetLookAt(Vec3& eye, Vec3& focus, Vec3& up);
	

	//�r���[�s��擾
	DirectX::XMFLOAT4X4& GetView() { return view; }

	//�v���W�F�N�V�����s��擾
	DirectX::XMFLOAT4X4& GetProjection() { return projection; }

	Vec3& GetEye() { return eye; }
	Vec3& GetFocus() { return focus; }
	Vec3& GetUp() { return up; }
	Vec3& GetFront() { return front; }
	Vec3& GetRight() { return right; }

	void SetEye(DirectX::XMFLOAT3 e) { eye = e; }

private:

	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	Vec3 eye;
	Vec3 focus;

	Vec3 up;
	Vec3 front;
	Vec3 right;
};