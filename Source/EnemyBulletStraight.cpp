#include "EnemyBulletStraight.h"
#include "Camera/CameraManager.h"

EnemyBulletStraight::EnemyBulletStraight(ID3D11Device* device, EnemyBulletManager* manager):EnemyBullet(manager)
{	
	model = new Model(device,"Data/Models/Bullet/bullet.fbx",true,0);
	// 表示サイズを調整
	scale.x = scale.y = scale.z = 1.0f;	

	debugRenderer = std::make_unique<DebugRenderer>(device);
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
	bulletSpeed = this->speed * elapsedTime;
	position.x += direction.x * bulletSpeed;
	position.y += direction.y * bulletSpeed;
	position.z += direction.z * bulletSpeed;
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

	//// 必要なったら追加
	debugRenderer.get()->DrawSphere(position, radius, Vec4(1, 0.5f, 0, 1));

	debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}

// 発射処理
void EnemyBulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
