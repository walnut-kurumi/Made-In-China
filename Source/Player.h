#pragma once

#include "Character.h"
#include "DebugRenderer.h"
#include "SBManager.h"
#include "Cost.h"
#include "Graphics/ConstantBuffer.h"

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
		SB,		 //　シフトブレイク
		Throw,	 //　シフトブレイク投擲
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

	float GetSlowTimer() { return cost.GetCost(); }
	float GetSlowMax() { return cost.GetMaxCost(); }

	float GetPlaybackSpeed() { return slow ? slowSpeed : playbackSpeed; }
	bool GetSlowFlag() { return slow; }

	void SetHitstop(bool hit) { hitstop = hit; }
	bool GetHitstop() { return hitstop; }
	float GetHitStopSpeed() { return hitstop ? hitstopSpeed : playbackSpeed; }
	
	bool GetClock() { return clock; }

	bool GetInvincible() { return invincible; }

	float GetBlurPower() { return blurPower; }

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
	bool InputSB();

	// 通常攻撃&パリィ
	bool InputAttack();
	
	// 死亡した時に呼ばれる
	void OnDead() override { isDead = true; }

	void Vibration(float elapsedTime);

	bool Raycast(Vec3 move);

	void SBManagement(float elapsedTime);

	// 中心座標更新
	void UpdateCenterPosition();

protected:
	void OnLanding() override;

	// 状態遷移
	// 待機ステート
	void TransitionIdleState();
	void UpdateIdleState(float elapsedTime);

	// 走るステート
	void TransitionRunState();
	void UpdateRunState(float elapsedTime);

	// ジャンプ
	void TransitionJumpState();
	void UpdateJumpState(float elapsedTime);

	// 攻撃ステート
	void TransitionAttackState();
	void UpdateAttackState(float elapsedTime);

	// SB投擲ステート
	void TransitionSBThrowState();
	void UpdateSBThrowState(float elapsedTime);

	// SBステート
	void TransitionSBState();
	void UpdateSBState(float elapsedTime);

	// フィニッシャーステート
	void TransitionFinisherState();
	void UpdateFinisherState(float elapsedTime);


private:
	template<class Type, typename Return, typename ...Args>
	using Temp = Return(Type::*)(Args...);
	Temp<Player, void, float> UpdateState[static_cast<int>(AnimeState::End)];
	
	AnimeState state = AnimeState::Idle;

	ConstantBuffer<Destruction> destructionCb{};

private:
	std::unique_ptr<DebugRenderer> debugRenderer;

	// デストラクションシェーダ用
	Destruction dest{};

	// 攻撃判定用 体の位置
	Vec3 atkPos{};
	Vec3 waistPos{};
	Vec3 headPos{};

	// パッド振動
	const int MAX_SPEED = 65535;                  
	const int MIN_SPEED = 0;              
	bool vibration = false;
	float vibTimer = 0.0f;

	float moveSpeed = 0.0f;
	float turnSpeed = DirectX::XMConvertToRadians(3600);

	// 攻撃関連
	bool atk = false;
	float atkRadius = 0.0f;
	float atkTimer = 0.0f;
	float atkPower = 0.0f;

	// ジャンプ関連
	float jumpSpeed = 0.0f;
	int jumpCount = 0;
	const int jumpLimit = 3;

	// スローモーション用
	float playbackSpeed = 0.0f;
	float slowSpeed = 0.0f;
	bool slow = false;
	// ヒットストップ用
	float hitstopSpeed = 0.0f;
	bool hitstop = false; // 攻撃当たったらtrue
	// SB用
	bool weapon = true;		// 武器を持っているか否か
	bool sbhit = false;		// 敵に対するフィニッシャー
	float sbSpeed = 0;			// sb移動速度
	float sbSpace = 0;			// 敵の後ろ一定距離
	Vec3 sbdir = { 0,0,0 };	// 向き
	Vec3 sbPos = { 0,0,0 };	// 位置
	Vec3 sbStartPos = { 0,0,0 };	// デストラクション用SBスタート位置
	int sbHitEmy = -1;	// SBがヒットした敵（）当てた敵は確実に倒す用
	const float sbCost = 2.0f;
	bool invincible = false;	// 無敵状態（SB）
	float blurPower = 0.0f;// 移動ブラー力

	bool clock = false;	 // プレイヤー以外の時間
	// SB時間制限
	float sbTimer = 0.0f;
	const float sbMaxTime = 0.5f;
	// 死亡
	bool isDead = false;



	Cost cost;
};
