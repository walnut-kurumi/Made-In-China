#include "EnemyBullet.h"
#include "EnemyBulletManager.h"
#include "Graphics/Graphics.h"
#include "StageManager.h"

EnemyBullet::EnemyBullet(EnemyBulletManager* manager) :manager(manager)
{
	manager->Register(this);
}

// 破棄
void EnemyBullet::Destroy()
{
	manager->Remove(this);
}

void EnemyBullet::CollisionVsStage()
{

	// レイの開始位置と終点位置
	DirectX::XMFLOAT3 start = { position.x , position.y, position.z };
	DirectX::XMFLOAT3 end = { position.x + direction.x, position.y + direction.y, position.z + direction.z };

	// レイキャストによる壁判定
	HitResult hit;	

	if (StageManager::Instance().RayCast(start, end, hit)) 
	{
		Destroy();
	}
}

// 行列更新処理
void EnemyBullet::UpdateTransform()
{
	DirectX::XMVECTOR Front, Up, Right;
	// 前ベクトルを算出
	Front = DirectX::XMLoadFloat3(&direction);
	Front = DirectX::XMVector3Normalize(Front);
	// 仮の上ベクトルを算出
	Up = DirectX::XMVectorSet(0.001f, 1, 0, 0);
	Up = DirectX::XMVector3Normalize(Up);
	// 右ベクトルを算出
	Right = DirectX::XMVector3Cross(Up, Front);
	// 上ベクトルを算出
	Up = DirectX::XMVector3Cross(Front, Right);
	// 計算結果を取り出し
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);
	// 算出した軸ベクトルから行列を作成
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
