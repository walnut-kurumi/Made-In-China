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

	// �ړ����͏���
	bool InputMove(float elapsedTime);
	// �������
	bool Step(float elapsedTime, float leftRight, float backFront);
	
	// ���S�������ɌĂ΂��
	void OnDead() override { isDead = true; }

protected:

	// ��ԑJ��
	// �ҋ@�X�e�[�g
	void TransitionIdleState();
	void UpdateIdleState(float elapsedTime);

	// �ړ��X�e�[�g
	void TransitionWalkState();
	void UpdateWalkState(float elapsedTime);

	// ����X�e�[�g
	void TransitionRunState();
	void UpdateRunState(float elapsedTime);

	// �X�e�b�v����X�e�[�g�i�������j
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
