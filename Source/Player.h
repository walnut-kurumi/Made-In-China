#pragma once

#include "Character.h"
#include "DebugRenderer.h"

class Player : public Character
{
private:

	enum class State
	{
		Idle,
		Run,
		Walk,
		Jump,
		End,
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

private:

	// 移動入力処理
	bool InputMove(float elapsedTime);
	// 回避処理
	bool Step(float elapsedTime, float leftRight, float backFront);
	
	// 死亡した時に呼ばれる
	void OnDead() override { isDead = true; }

protected:

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

	bool aim = false;
	float stepSpeed = 60.0f;

	std::unique_ptr<DebugRenderer> debugRenderer;
	SkinnedMesh* skinned;

	bool isDead = false;
};
