#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

Enemy::Enemy(ID3D11Device* device)
{
	debugRenderer = std::make_unique<DebugRenderer>(device);
}

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{	
	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// 破棄
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}