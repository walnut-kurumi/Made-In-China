#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Vec.h"
#include <memory>
#include <vector>
#include "DebugRenderer.h"
#include "Effect.h"

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
	void SetDirection(Vec3 d)  { direction = d ; }
	// 方向取得
	const Vec3& GetDirection() const { return direction; }
	// スケール取得
	const Vec3& GetScale() const { return scale; }
	// 半径取得
	float GetRadius() const { return radius; }
	// パリィフラグ設定
	void SetReflectionFlag(bool r) { reflectionFlag = r; }
	// パリィフラグ取得
	bool GetReflectionFlag() const { return reflectionFlag; }

	// 動かすかどうか
	void SetIsMove(bool b) { isMove = b; }

	// 破棄
	void Destroy();

	// 弾がステージに当たったら消す
	void CollisionVsStage();

protected:
	// 行列更新処理
	void UpdateTransform();

protected:

	// デバッグ
	std::unique_ptr<DebugRenderer> debugRenderer;

	Vec3	position = { 0,0,0 };
	Vec3	direction = { 0,0,1 };
	Vec3	scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	std::unique_ptr<EnemyBulletManager> manager = nullptr;

	float radius = 1.5f;
	float speed = 120.0f;
	float bulletSpeed = 0.0f;

	// パリィした弾かどうか -> 弾き返してたら敵に当たるようになる
	bool reflectionFlag = false;
	
	// 動くかどうか
	bool isMove = true;
	
};
