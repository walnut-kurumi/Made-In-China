#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Graphics/Vec.h"
using namespace DirectX;

class Camera
{
public:
	//パースペクティブ設定
	void SetPerspectiveFov(float fov, float aspect, float nearZ, float farZ);
	//視点・注視点設定
	void SetLookAt(Vec3& eye, Vec3& focus, Vec3& up);
	

	//ビュー行列取得
	DirectX::XMFLOAT4X4& GetView() { return view; }

	//プロジェクション行列取得
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