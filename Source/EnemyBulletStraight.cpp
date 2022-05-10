#include "EnemyBulletStraight.h"
#include "Camera/CameraManager.h"

EnemyBulletStraight::EnemyBulletStraight(ID3D11Device* device, EnemyBulletManager* manager):EnemyBullet(manager)
{	
	model = std::make_unique<Model>(device,"Data/Models/Bullet/bullet.fbx",true,0);
	// 表示サイズを調整
	scale.x = scale.y = scale.z = 1.0f;	

	ballisticEffect = new Effect("Data/Effect/enemyBullet.efk");

	debugRenderer = std::make_unique<DebugRenderer>(device);
}

EnemyBulletStraight::~EnemyBulletStraight()
{	
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
		ballisticEffect->Stop(handle);
	}

	// 移動
	bulletSpeed = this->speed * elapsedTime;
	position.x += direction.x * bulletSpeed;
	position.y += direction.y * bulletSpeed;
	position.z += direction.z * bulletSpeed;

	// エフェクト座標
	ballisticEffect->SetPosition(handle, position);

	// オブジェクト行列を更新
	UpdateTransform();
	// モデル行列更新
	model->UpdateTransform(transform);

	CollisionVsStage();
}
// 描画処理
void EnemyBulletStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	model->Begin(dc, *shader);
	model->Render(dc);


#ifdef _DEBUG
	//// 必要なったら追加
	debugRenderer.get()->DrawSphere(position, radius, Vec4(1, 0.5f, 0, 1));

	debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
#endif
}

// 発射処理
void EnemyBulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{	

	this->direction = direction;
	this->position = position;

	float angle = 90;
	if (direction.x > 0)angle = -90;
	else angle = 90;
	float radian = DirectX::XMConvertToRadians(angle);
	handle = ballisticEffect->PlayDirection(position, 1.0f, radian);
	ballisticEffect->SetSpeed(handle, 8.0f);
}
