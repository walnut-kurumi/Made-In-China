#include "EnemyBulletStraight.h"

EnemyBulletStraight::EnemyBulletStraight(ID3D11Device* device, EnemyBulletManager* manager):EnemyBullet(manager)
{	
	model = new Model(device,"Data/Models/Bullet/bullet.fbx",true,0);

	// 表示サイズを調整
	scale.x = scale.y = scale.z = 0.05f;	
}

EnemyBulletStraight::~EnemyBulletStraight()
{
	delete model;
}
// 更新処理
void EnemyBulletStraight::Update(float elapsedTime)
{
	// 寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f)
	{
		// 自分を削除
		Destroy();
	}
	// 移動
	float speed = this->speed * elapsedTime;
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;
	// オブジェクト行列を更新
	UpdateTransform();
	// モデル行列更新
	model->UpdateTransform(transform);
}
// 描画処理
void EnemyBulletStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	model->Begin(dc, *shader);
	model->Render(dc);
}

// 発射処理
void EnemyBulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
