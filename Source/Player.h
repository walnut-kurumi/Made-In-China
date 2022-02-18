#pragma once

#include "Character.h"
#include "DebugRenderer.h"

class Player : public Character
{
private:

	enum class State
	{
		Idle,	//　待機
		Run,	//　走り
		Walk,	//　歩き
		Jump,	//　ジャンプ開始
		Fall,	//　落下
		Land,	//　着地
		End,	//　お　わ　り　💛
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

	float GetPlaybackSpeed() { return playbackSpeed; }

private:

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	void InputJump();

	// スローモーション入力処理
	void InputSlow();

	// SB入力処理
	void InputSB();
	
	// 死亡した時に呼ばれる
	void OnDead() override { isDead = true; }

protected:
	void OnLanding() override;

	// 状態遷移
	// 待機ステート
	void TransitionIdleState();
	void UpdateIdleState(float elapsedTime);

	// 移動ステート
	void TransitionWalkState();
	void UpdateWalkState(float elapsedTime);

	// 走るステート
	void TransitionRunState();
	void UpdateRunState(float elapsedTime);

	// ステップ回避ステート（横方向）
	void TransitionJumpState();
	void UpdateJumpState(float elapsedTime);




private:
	template<class Type, typename Return, typename ...Args>
	using Temp = Return(Type::*)(Args...);
	Temp<Player, void, float> UpdateState[static_cast<int>(State::End)];
	
	State state = State::Idle;

private:
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 20.0f;
	int jumpCount = 0;
	int jumpLimit = 1;

	std::unique_ptr<DebugRenderer> debugRenderer;
	SkinnedMesh* skinned;

	float playbackSpeed = 1.0f;

	bool isDead = false;
};
