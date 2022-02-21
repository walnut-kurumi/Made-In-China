#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "EnemyBulletManager.h"

// �G�l�~�[
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}
	
	// ����������
	virtual void Init() = 0;
	// �X�V����
	virtual void Update(float elapsedTime) = 0;
	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;	

	// �j��
	void Destroy();

	void SetLengthSq(float sq) { lengthSq = sq; }
	float GetLengthSq()const { return lengthSq; }

protected:
	float lengthSq = 0;

	// �e�}�l
	EnemyBulletManager bulletManager;

	// �f�o�b�O
	std::unique_ptr<DebugRenderer> debugRenderer;
};