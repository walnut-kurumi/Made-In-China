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
		position.y = position.z = 0.0f;
		isDead = true;
	}
}
