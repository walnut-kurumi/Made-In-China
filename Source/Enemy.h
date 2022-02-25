#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "EnemyBulletManager.h"
#include "Player.h"

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

	void SetPlayer(Player* p) { player = p; }
	
protected:
	float lengthSq = 0;

	Player* player = nullptr;

	// �f�o�b�O
	std::unique_ptr<DebugRenderer> debugRenderer;

	// �}�e���A���J���[
	Vec4 materialColor = { 1,1,1,1 };
};