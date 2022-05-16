#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"


// �j��
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

// ���׼�
void Enemy::FallIsDead()
{
	if (position.y < -100.0f)
	{
		position = { 0,0,0 };
		isDead = true;
	}
}
