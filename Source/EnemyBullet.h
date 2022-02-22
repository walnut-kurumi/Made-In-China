#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Vec.h"

// 前方宣言
class EnemyBulletManager;

// 弾丸
class EnemyBullet
{
public:
	EnemyBullet(EnemyBulletManager* manager);
	virtual ~EnemyBullet() {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;	
	// 位置取得
	const Vec3& GetPosition() const { return position; }
	// 方向取得
	const Vec3& GetDirection() const { return direction; }
	// スケール取得
	const Vec3& GetScale() const { return scale; }
	// 半径取得
	float GetRadius() const { return radius; }
	// 破棄
	void Destroy();

protected:
	// 行列更新処理
	void UpdateTransform();

protected:
	Vec3	position = { 0,0,0 };
	Vec3	direction = { 0,0,1 };
	Vec3	scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	EnemyBulletManager* manager = nullptr;

	float radius = 0.5f;
};
