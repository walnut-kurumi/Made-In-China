#pragma once

#include "Character.h"
#include "DebugRenderer.h"
#include "SBManager.h"

class Player : public Character
{
private:

	enum class AnimeState
	{
		Idle,	 //　待機
		Run,	 //　走り
		Attack,  // 攻撃
		Jump,	 //　ジャンプ開始
		Finisher,// フィニッシャー
		Fall,	 //　落下
		Land,	 //　着地
		End,	 //　お　わ　り　💛 ふぁっ〇ゅ～～～！！
	};

public:
	Player(ID3D11Device* device);
	~Player()override;

	void Init();
	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);

	void DrawDebugGUI();

	Vec3 GetMoveVec() const;

	bool GetDead() { return isDead; }

	float GetSlowTimer() const { return slowTimer; }

	float GetPlaybackSpeed() { return slow ? slowSpeed : playbackSpeed; }
	bool GetSlowFlag() { return slow; }

	void SetHitstop(bool hit) { hitstop = hit; }
	bool GetHitstop() { return hitstop; }
	float GetHitStopSpeed() { return hitstop ? hitstopSpeed : playbackSpeed; }

	void CollisionPanchiVsEnemies();
	void CollisionPanchiVsProjectile();
	void CollisionSBVsEnemies();
	void CollisionSBVsStage();

private:

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	bool InputJump();

	// スローモーション入力処理
	void InputSlow(float elapsedTime);

	// SB入力処理
	void InputSB();

	// 通常攻撃&パリィ
	bool InputAttack();
	
	// 死亡した時に呼ばれる
	void OnDead() override { isDead = true; }

protected:
	void OnLanding() override;

	// 状態遷移
	// 待機ステート
	void TransitionIdleState();
	void UpdateIdleState(float elapsedTime);

	// 走るステート
	void TransitionRunState();
	void UpdateRunState(float elapsedTime);

	// ステップ回避ステート（横方向）
	void TransitionJumpState();
	void UpdateJumpState(float elapsedTime);

	// 攻撃ステート
	void TransitionAttackState();
	void UpdateAttackState(float elapsedTime);

	// フィニッシャーステート
	void TransitionFinisherState();
	void UpdateFinisherState(float elapsedTime);


private:
	template<class Type, typename Return, typename ...Args>
	using Temp = Return(Type::*)(Args...);
	Temp<Player, void, float> UpdateState[static_cast<int>(AnimeState::End)];
	
	AnimeState state = AnimeState::Idle;

private:
	std::unique_ptr<DebugRenderer> debugRenderer;
	SkinnedMesh* skinned;

	// 攻撃判定用 体の位置
	Vec3 atkPos{};
	Vec3 waistPos{};
	Vec3 headPos{};

	float moveSpeed = 0.0f;
	float turnSpeed = DirectX::XMConvertToRadians(3600);

	// 攻撃関連
	bool atk = false;
	float atkRadius = 0.0f;
	float atkTimer = 0.0f;
	float atkImpulse = 0.0f;

	// ジャンプ関連
	float jumpSpeed = 0.0f;
	int jumpCount = 0;
	const int jumpLimit = 3;

	// スローモーション用
	float playbackSpeed = 0.0f;
	float slowSpeed = 0.0f;
	bool slow = false;
	// スローモーションの時間
	const float slowMax = 10.0f;
	float slowTimer = slowMax;
	const float CTMax = 2.5f;
	float slowCTTimer = CTMax;
	// クールタイム　0:無し 1:CT明け直後 2:CT中
	int slowCT = 0;
	// ヒットストップ用
	float hitstopSpeed = 0.0f;
	bool hitstop = false; // 攻撃当たったらtrue
	// SB用
	bool weapon = true;		// 武器を持っているか否か
	bool finish = false;	// 敵に対するフィニッシャー
	float backDir;			// 敵の後ろ一定距離
	// 死亡
	bool isDead = false;
    // 体の向き
	bool direction = false;
};
