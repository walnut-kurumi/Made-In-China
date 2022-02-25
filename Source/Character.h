#pragma once

#include "Graphics/Vec.h"
#include "Model.h"

class Character
{
public:
	Character() {}
	virtual ~Character() {}

	// �s��X�V����
	virtual void UpdateTransform();

	const Vec3& GetPosition() const { return position; }
	void SetPosition(const Vec3& position) {
		this->position = position;
		saveposition = position;
	}
	const Vec3& GetAngle() const { return angle; }
	void SetAngle(const Vec3& angle) { this->angle = angle; }
	const Vec3& GetScale() const { return scale; }
	void SetScale(const Vec3& scale) { this->scale = scale; }
	const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	const Vec3& GetCenterPosition() const { return centerPosition; }

	const Vec3& GetVelocity() const { return velocity; }

	// ��������
	float GetHeight() const { return height; }

	// ���a�擾
	float GetWidth() const { return width; }

	// �n�ʂɐڒn���Ă��邩
	bool Ground() const { return isGround; }

	// ���N��Ԃ��擾
	int GetHealth() const { return health; }

	//	�ő匒�N��Ԃ��擾
	int GetMaxHealth() const { return maxHealth; }

	DirectX::XMFLOAT4X4 GetRota() const { return rotaStans; }

	// �Ռ���^����
	void AddImpulse(const Vec3& impulse);

	// �_���[�W��^����
	bool ApplyDamage(int damage, float invincibleTime);

	float GetRadius() const { return radius; }

	// �ǂݍ��񂾃��f���Z�b�g
	void SetModel(Model* md) { model = md; }


private:
	// �������͍X�V����
	void UpdateVerticalVelocitiy(float elapsedFrame);
	// �����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);

	// �������͍X�V����
	void UpdateHorizontalVelocity(float elapsedFrame);
	// �����ړ��X�V����
	void UpdateHorizontalMove(float elapsedTime);

	// ���͍X�V����
	void UpdateVelocity(float elapsedFrame);
	// �ړ��X�V����
	void UpdateMove(float elapsedTime);


protected:
	void Move(float vx, float vz, float speed);
	void AttackMove(float vx, float vy, float speed);
	void Turn(float elapsedTime, float vx, float vz, float speed);
	void Jump(float speed);
	void UpdateSpeed(float elapsedTime);
	void UpdateInvincibleTimer(float elapsedTime);

	virtual void OnLanding() {}

	// �_���[�W���󂯂����ɌĂ΂��
	virtual void OnDamaged() {}

	// ���S�������ɌĂ΂��
	virtual void OnDead() {}	

protected:
	Model* model{};

	Vec3 position = { 0,0,0 };
	Vec3 saveposition = { 0,0,0 };
	Vec3 centerPosition = { 0,0,0 };
	Vec3 posMae = {};
	Vec3 posAto = {};
	Vec3 angle = { 0,0,0 };
	Vec3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	Vec3 normal = { 0,0,0 };
	Vec4 color = { 0,0,0,0 };


	Vec3 dir{};


	Vec4 quaternion{};
	Vec4 quaternionStans{};
	DirectX::XMFLOAT4X4 rotaStans{};

	float gravity = -1.0f;

	bool gravFlag = true;


	Vec3 velocity = { 0,0,0 };
	float downMax = -60.0f;

	bool isGround = false;
	float height = 4.0f;
	float width = 4.0f;

	int health = 5;
	int maxHealth = 5;
	float invincibleTimer = 0.0f;
	float friction = 0.8f;
	float acceleration = 1.0f;
	float maxMoveSpeed = 160.0f;
	float moveVecX = 0.0f;
	float moveVecY = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
	float stepOffset = 1.0f;
	float slopeRate = 1.0f;
	float radius = 0.5f;

	bool deathFlag = false;
};