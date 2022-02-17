#include "Camera.h"

void Camera::SetPerspectiveFov(float fov, float aspect, float nearZ, float farZ) {
	 DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ));
}

void Camera::SetLookAt(Vec3& eye, Vec3& focus, Vec3& up) {
	//view行列
	DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&eye);			//eyePos
	DirectX::XMVECTOR F = DirectX::XMLoadFloat3(&focus);		//focusPos
	DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&up);			//upDirection	
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(E, F, U);	//view
	DirectX::XMStoreFloat4x4(&view, V);

	//ビューを逆行列化し、ワールド座標に戻す
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, V);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);
	
	//ワールド座標ので向きを取得
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	this->eye = eye;
	this->focus = focus;
}