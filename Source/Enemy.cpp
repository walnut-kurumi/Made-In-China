#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"


// ”jŠü
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

// µÁÀ×¼Ç
void Enemy::FallIsDead()
{
	if (position.y < -100.0f)
	{
		PositionInitialize();
		isDead = true;
	}
}
