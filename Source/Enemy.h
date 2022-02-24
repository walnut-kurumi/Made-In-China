#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "EnemyBulletManager.h"

// エネミー
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}
	
	// 初期化処理
	virtual void Init() = 0;
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;	

	// 破棄
	void Destroy();

	void SetLengthSq(float sq) { lengthSq = sq; }
	float GetLengthSq()const { return lengthSq; }

	void SetPlayerPos(Vec2 player) { playerPos = player; };
	void SetPlayerATKDirection(Vec2 pAtkD) { playerAttackDirection = pAtkD; };

protected:
	float lengthSq = 0;

	// 弾マネ
	EnemyBulletManager bulletManager;

	// プレイヤーの座標
	Vec2 playerPos = {};
	// プレイヤーの攻撃方向
	Vec2 playerAttackDirection = {};

	// デバッグ
	std::unique_ptr<DebugRenderer> debugRenderer;
};