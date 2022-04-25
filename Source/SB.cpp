#include "SB.h"
#include "SBManager.h"
#include "Graphics/Graphics.h"

SB::SB(SBManager* manager) :manager(manager) {
	manager->Register(this);
}

// 破棄
void SB::Destroy() {
	manager->Remove(this);
}

// 行列更新処理
void SB::UpdateTransform() {
	DirectX::XMVECTOR Front, Up, Right;

	//前ベクトル算出
	Front = DirectX::XMLoadFloat3(&direction);
	Front = DirectX::XMVector3Normalize(Front);
	//仮の上ベクトル算出
	Up = DirectX::XMVectorSet(0.001f, 1, 0, 0);
	Up = DirectX::XMVector3Normalize(Up);
	//右ベクトル算出
	Right = DirectX::XMVector3Cross(Up, Front);
	Right = DirectX::XMVector3Normalize(Right);
	//上ベクトル算出
	Up = DirectX::XMVector3Cross(Front, Right);


	//計算結果取り出し
	DirectX::XMFLOAT3 front, up, right;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	//算出したベクトルから行列作成
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0.0f;
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0.0f;
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0.0f;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;
	// 発射方向
	this->direction = front;
}
