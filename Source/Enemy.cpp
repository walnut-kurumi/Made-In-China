#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

Enemy::Enemy(ID3D11Device* device)
{
	debugRenderer = std::make_unique<DebugRenderer>(device);
}

// �f�o�b�O�v���~�e�B�u�`��
void Enemy::DrawDebugPrimitive()
{	
	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// �j��
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}