#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "EnemyBulletManager.h"
#include "Player.h"
#include "Effect.h"
#include "DoorManager.h"

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

	void SetPlayer(Player* p) { player = p; }
	
	// 死亡した時に呼ばれる
	void OnDead() override { isDead = true; }
	
	// 攻撃予兆を出すかどうか
	bool GetIsAttack() { return isAttack; }

	// 初期座標セット
	void SetInitialPos(Vec3 pos) { initialPos = pos; }
	// 座標初期化
	void PositionInitialize() { position = initialPos; }

	// グループ番号セット
	void SetGroupNum(int g) { groupNum = g; }
	// グループ番号ゲット
	int GetGroupNum() { return groupNum; }
	// 初期化用
	void SetInitialGroupNum(int g) { initialGroupNum = g; }
	// 初期化
	void GroupNumInitialize() { groupNum = initialGroupNum; }


	// プレイヤーを見つけているかのフラグゲット
	void SetIsSearch(bool s) { isSearch = s; }
	// プレイヤーを見つけているかのフラグゲット
	bool GetIsSearch() { return isSearch; }

	// 徘徊するかどうか
	void SetWalkFlag(bool b) { walk = b; }
	// 初期化用
	void SetInitialWalk(bool b) { initialWalk = b; }
	// 初期化
	void WalkFlagInitialize() { walk = initialWalk; }	

	// 最初の向き ←左true　false右→
	void SetInitialDirection(bool b) { initialDirection = b; }
	void DirectionInitialize() { direction = initialDirection; }

	// ｵﾁﾀﾗｼﾇ
	void FallIsDead();

protected:
	// 自分が何番のグループか
	int groupNum = 0;
	int initialGroupNum = 0;	

	// プレイヤーを見つけているか
	bool isSearch = false;
	// 歩き回るかどうか
	bool walk = false;
	bool initialWalk = false;


	// 攻撃予兆Flag
	bool isAttack = false;

	// 死んでるか
	bool isDead = false;
	
	// 移動する向き ←左true　false右→
	bool initialDirection = false;
	bool direction = false;


	// 初期座標
	Vec3 initialPos = {};

	float lengthSq = 0;

	// プレイヤー
	Player* player = nullptr;

	// デバッグ
	std::unique_ptr<DebugRenderer> debugRenderer;

	// マテリアルカラー
	Vec4 materialColor = { 1,1,1,1 };


	// エフェクト
	Effect* deadEffect = nullptr;	
	Effekseer::Handle handle = 0;
};