#pragma once

#include "Model.h"
#include "Collision.h"
#include "DebugRenderer.h"
#include "Framework.h"
#include "Player.h"
#include "audio/AudioResource.h"
#include "audio/Audio.h"

class Door
{

public:
	Door(ID3D11Device* device);
	~Door();

	void Init();

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime);
	void RenderGui();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	const Vec3& GetPos() const { return position; }
	void SetPos(const Vec3& position) { this->position = position; }
	const Vec3& GetAngle() const { return angle; }
	void SetAngle(const Vec3& angle) { this->angle = angle; }
	const Vec3& GetScale() const { return scale; }
	void SetScale(const Vec3& scale) { this->scale = scale; }

	void OpenTheDoor();
	void UpdateCollisionPos();

	// �h�A�Ƃ̓����蔻��
	void CollisionPlayerAtkVsDoor();
	void CollisionPlayerVsDoor();
	void CollisionEnemyVsDoor();

	void PlayerData(Player* p) { player = p; }

protected:
	// �s��X�V����
	void UpdateTransform();

protected:
	Model* model;
	Model* backModel;

	Player* player = nullptr;

	Vec3 position = { 0, 0, 0 };
	Vec3 angle = { 0, 0 ,0 };
	Vec3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};


	// �J���Ă��邩
	bool isOpen = false;

	// �����蔻��p
	// �h�A���J����Ƃ��̔���p���W
	Vec3 collisionPos = { 0,0,0 };
	// �h�A�̒��S���W
	Vec3 centerPos = { 0,0,0 };
	float radius = 0.0f;
	float attackRadius = 0.0f;

	// �v���C���[���߂Â����瓧���x�グ��������
	Vec4 backModelColor = { 0.75f,0.35f,0.95f,1.0 };
	float backModelRadius = 30.0f;
	float backModelAlpha = 1.0f;
	float modelAlpha = 1.0f;
	float radian = 0.0f;
	float radian2 = 0.0f;
	bool isBlinking = false;

	std::unique_ptr<DebugRenderer> debugRenderer;
	bool sedoor = false;
	std::unique_ptr<AudioSource> SEDoorB;
};