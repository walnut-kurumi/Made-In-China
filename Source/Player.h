#pragma once

#include "Character.h"
#include "DebugRenderer.h"
#include "SBManager.h"
#include "Cost.h"
#include "Graphics/ConstantBuffer.h"
#include "Effect.h"

#include "AfterimageManager.h"
#include "audio/AudioResource.h"
#include "audio/Audio.h"

class Player : public Character
{
private:

	enum class AnimeState
	{
		Idle,	 // 待機
		Run,	 // 走り
		Attack,  // 攻撃
		Jump,	 // ジャンプ開始
		Finisher,// フィニッシャー
		Fall,	 // 落下
		Land,	 // 着地
		SB,		 // シフトブレイク
		Throw,	 // シフトブレイク投擲
		Death,	 // 死亡
		End,	 // お　わ　り　💛 ふぁっ〇ゅ～～～！！
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
	bool GetReset() { return reset; }

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

	bool GetIsHit() { return isHit; }
	void SetIsHit(bool h) { isHit = h; }

	void SetIsControl(bool b) { isControl = b; }
	void SetCanSlow(bool b) { canSlow = b; }
	bool GetCanSlow() { return canSlow; }
	void SetCanAttack(bool b) { canAttack = b; }
	void SetSlowFixation(bool b) { slowFixation = b; }
	void SetIsTutorial(bool b) { isTutorial = b; }

	void CollisionPanchiVsEnemies();
	void CollisionPanchiVsProjectile();
	void CollisionSBVsEnemies();
	void CollisionSBVsStage();

	Vec3 GetAttackPosistion() const { return atkPos + position + waistPos; }
	bool GetIsAtk() const { return atk; }
	float GetAtkRadius() const { return atkRadius; }
	float GetSlowAlpha() const { return slowAlpha; }	


	bool GetIsControl() const { return isControl; }
	bool GetCanSlow() const { return canSlow; }
	bool GetCanAttack() const { return canAttack; }	
	bool GetisTutorial() const { return isTutorial; }

	void SetSt3(bool s) { st3flag = s; }
	bool GetSt3() const { return st3flag; }

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
	// SB移動用のレイキャスト
	bool Raycast(Vec3 move);
	// シフトブレイク全処理
	void SBManagement(float elapsedTime);
	// 中心座標更新
	void UpdateCenterPosition();
	// SB投げる処理
	void Launch(const Vec3& direction);

	// ｵﾁﾀﾗｼﾇ
	void FallIsDead();

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

	// 死亡ステート
	void TransitionDeathState();
	void UpdateDeathState(float elapsedTime);

private:
	template<class Type, typename Return, typename Args>
	using Temp = Return(Type::*)(Args);
	Temp<Player, void, float> UpdateState[static_cast<int>(AnimeState::End)];
	
	AnimeState state = AnimeState::Idle;

	ConstantBuffer<Destruction> destructionCb{};

private:
	std::unique_ptr<DebugRenderer> debugRenderer;

	// デストラクションシェーダ用
	Destruction dest{};

	// 攻撃判定用 体の位置
	Vec3 atkPos{};
	Vec3 swordPos{};
	Vec3 waistPos{};
	Vec3 headPos{};
	Vec3 sbLaunchPos{};

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
	float slowAlpha = 0.0f;
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
	int sbHitEmy = -1;	// SBがヒットした敵（）当てた敵は確実に倒す用
	float sbEraseLen = 0.0f; // シフトブレイクが消える距離
	const float sbCost = 0.75f;
	bool invincible = false;	// 無敵状態（SB）
	float blurPower = 0.0f; // 移動ブラー力
	float blurMax = 13.0f;	// ブラー最大値
	float blur = 0.0f;		// ブラー増減スピード
	float stopTime = 0.0f;	// シフトブレイク後の停止時間
	float stopTimer = 0.0f;	// シフトブレイク後の停止時間タイマー

	bool clock = false;	 // プレイヤー以外の時間
	// 死亡
	bool isDead = false;
	bool reset = false;
	// 攻撃くらった判定用
	bool isHit = false;


	// 攻撃えふぇくと
	Effect* hitEffect = nullptr;
	Effekseer::Handle handle = 0;
	
	// チュートリアル用変数
	bool isControl = true;
	bool canSlow = true;
	bool canAttack = true;
	bool slowFixation = false;	// スロー固定
	bool isTutorial = false;

	//st3かどうか
	bool st3flag = false;

	Cost cost;

	// 残像
	AfterimageManager* afterimageManager{};

	//audio
	bool deathse = false;
	bool slowse = false;
	std::unique_ptr<AudioSource> SEDeath;
	std::unique_ptr<AudioSource> SEJump;
	std::unique_ptr<AudioSource> SEAttack;
	std::unique_ptr<AudioSource> SEReflect;
	std::unique_ptr<AudioSource> SESBstart;
	std::unique_ptr<AudioSource> SEMove;
	std::unique_ptr<AudioSource> SESlowStart;
};