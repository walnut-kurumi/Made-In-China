#pragma once

#include "Graphics/Vec.h"
#include "Model.h"

class Character
{
public:
	Character() {}
	virtual ~Character() {}

	// 行列更新処理
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

	// 高さ処理
	float GetHeight() const { return height; }

	// 半径取得
	float GetWidth() const { return width; }

	// 地面に接地しているか
	bool Ground() const { return isGround; }

	// 健康状態を取得
	int GetHealth() const { return health; }
	void SetHealth(int hp) { health = hp; }

	//	最大健康状態を取得
	int GetMaxHealth() const { return maxHealth; }

	// 衝撃を与える
	void AddImpulse(const Vec3& impulse);

	// ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime);

	float GetRadius() const { return radius; }
	
	float GetStepOffset() const { return stepOffset; }

	void SetDirection(int i) { direction = i; }

private:
	// 垂直速力更新処理
	void UpdateVerticalVelocitiy(float elapsedFrame);
	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedFrame);
	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

	// 速力更新処理
	void UpdateVelocity(float elapsedFrame);
	// 移動更新処理
	void UpdateMove(float elapsedTime);


protected:
	void Move(float vx, float vz, float speed);
	void Turn(float elapsedTime, float vx, float vz, float speed);
	void Jump(float speed);
	void UpdateSpeed(float elapsedTime);
	void UpdateInvincibleTimer(float elapsedTime);

	virtual void OnLanding() {}

	// ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {}

	// 死亡した時に呼ばれる
	virtual void OnDead() {}		

protected:
	Model* model{};

	Vec3 position = { 0,0,0 };
	Vec3 saveposition = { 0,0,0 };
	Vec3 centerPosition = { 0,0,0 };
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

	float gravity = -3.0f;

	bool gravFlag = true;


	Vec3 velocity = { 0,0,0 };
	float downMax = -175.0f;

	bool isGround = false;
	float height = 4.0f;
	float width = 4.0f;

	int health = 5;
	int oldHealth = 0;
	int maxHealth = 5;
	float invincibleTimer = 0.0f;
	float friction = 5.0f;
	float acceleration = 8.0f;
	float maxMoveSpeed = 160.0f;
	float moveVecX = 0.0f;
	float moveVecY = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.2f;
	float stepOffset = 1.0f;
	float slopeRate = 1.0f;
	float radius = 0.5f;

	bool deathFlag = false;

	// -1が左 +1が右向き 0は移動なし
	int direction = 0;

	// 貫通床を貫通するか否か
	bool penetrate = false;
};