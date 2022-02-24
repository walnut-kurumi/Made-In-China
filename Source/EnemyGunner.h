#pragma once

#include "Model.h"
#include "Enemy.h"

// Gunner
class EnemyGunner : public Enemy
{
private:	
	enum class State
	{
		Idle,		// �ҋ@ �p�j�^�[�����ɏ�������
		Run,		// �ړG������˒������ɓ���܂�
		Walk,		// �p�j ��{����
		Attack,		// �˒������ɓ������玀�ʂ��˒��������痣���܂ōU��
		Blow,		// �v���_���[�W���󂯂��Ƃ��ɐ�����΂��Ƃ��̎p��
		Death,		// ������΂��ꂽ���Ƃ̎p��
		End,		// ���@��@��
	};


public:
	EnemyGunner(ID3D11Device* device);
	~EnemyGunner()override;

	void Init();
	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc,Shader* shader);	
	
	bool GetDead() { return isDead; }
	

private:	
	// �p�j  ����true�@false�E��
	void MoveWalk(bool direction );
	// ���G�@��������������true�Ԃ�
	bool Search();
	// �ړG
	void MoveRun(bool direction);
	// �˒������`�F�b�N �˒��������Ȃ�true�Ԃ��čU��
	bool CheckAttackRange();
	// �ˌ��U��
	void MoveAttack(float cooldown);
	// ������� �v���C���[�U���̕����ɐ������
	void MoveBlow();
	
	// ���S�������ɌĂ΂��
	void OnDead() override { isDead = true; }

protected:

	// ��ԑJ��
	 
	// �ҋ@�X�e�[�g
	void TransitionIdleState();
	void UpdateIdleState(float elapsedTime);
	// �ҋ@�^�C�}�[�X�V
	void IdleTimerUpdate(float elapsedTime);

	// �ړ��X�e�[�g
	void TransitionWalkState();
	void UpdateWalkState(float elapsedTime);
	// �ҋ@�^�C�}�[�X�V
	void WalkTimerUpdate(float elapsedTime);

	// ����X�e�[�g
	void TransitionRunState();
	void UpdateRunState(float elapsedTime);

	// �U���X�e�[�g
	void TransitionAttackState();
	void UpdateAttackState(float elapsedTime);
	// �U���N�[���_�E���X�V
	void AttackCooldownUpdate(float elapsedTime);

	// ������уX�e�[�g
	void TransitionBlowState();
	void UpdateBlowState(float elapsedTime);

	// ���S�X�e�[�g
	void TransitionDeathState();
	void UpdateDeathState(float elapsedTime);
	

private:
	template<class Type, typename Return, typename ...Args>
	using Temp = Return(Type::*)(Args...);
	Temp<EnemyGunner, void, float> UpdateState[static_cast<int>(State::End)];

	State state = State::Idle;

private:
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
			
	SkinnedMesh* skinned;

	bool isDead = false;

	// �ҋ@�p�^�C�}�[
	float idleTimer = 0;
	// �^�[���t���O
	bool turnFlag = false;

	// �����p�^�C�}�[(�^�[���܂ł̎���)
	float walkTimer = 0;
	// �����t���O
	bool walkFlag = false;	
	
	// ���G�͈́@�Z�`
	Vec2 searchAreaPos	 = {};
	Vec2 searchAreaScale = {};

	// �U���͈�
	float attackRange = 20.0f;
	// �U��CD
	float attackCooldown = 0;

	// ������΂�����
	float blowTimer = 0;	

	// �ړ��������
	bool direction = false;
};
