#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "EnemyBulletManager.h"
#include "Player.h"

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

	// グループ番号セット
	void SetGroupNum(int g) { groupNum = g; }
	// グループ番号ゲット
	int GetGroupNum() { return groupNum; }

	// プレイヤーを見つけているかのフラグゲット
	void SetIsSearch(bool s) { isSearch = s; }
	// プレイヤーを見つけているかのフラグゲット
	bool GetIsSearch() { return isSearch; }

	// 徘徊するかどうか
	void SetWalkFlag(bool b) { walk = b; }


protected:
	// 自分が何番のグループか
	int groupNum = 0;
	// プレイヤーを見つけているか
	bool isSearch = false;
	// 歩き回るかどうか
	bool walk = false;

	// 攻撃予兆Flag
	bool isAttack = false;

	// 死んでるか
	bool isDead = false;
	

	float lengthSq = 0;

	// プレイヤー
	Player* player = nullptr;

	// デバッグ
	std::unique_ptr<DebugRenderer> debugRenderer;

	// マテリアルカラー
	Vec4 materialColor = { 1,1,1,1 };
};