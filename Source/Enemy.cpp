#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"


// �j��
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}