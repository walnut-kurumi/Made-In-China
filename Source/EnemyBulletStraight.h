#pragma once

#include "Model.h"
#include "EnemyBullet.h"
#include "EnemyBulletManager.h"

// ���i�e��
class EnemyBulletStraight : public EnemyBullet
{
public:
	EnemyBulletStraight(ID3D11Device* device, EnemyBulletManager* manager);
	~EnemyBulletStraight() override;

	// �X�V����
	void Update(float elapsedTime) override;
	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader)override;
	// ����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float  speed = 50.0f;
	float lifeTimer = 3.0f;
};
