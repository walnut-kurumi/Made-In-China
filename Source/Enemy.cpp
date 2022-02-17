#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"


// ”jŠü
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}