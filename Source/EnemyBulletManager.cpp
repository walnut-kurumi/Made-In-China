#include "EnemyBulletManager.h"



EnemyBulletManager::~EnemyBulletManager()
{
	Clear();
}

// 更新処理
void EnemyBulletManager::Update(float elapsedTime)
{
	// 更新処理
	for (EnemyBullet* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}
	// 破棄処理
	// ※projectilesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (EnemyBullet* projectile : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<EnemyBullet*>::iterator it = std::find(projectiles.begin(), projectiles.end(),
			projectile);
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}
		// 弾丸の破棄処理
		delete projectile;
	}
	// 破棄リストをクリア
	removes.clear();
}
// 描画処理
void EnemyBulletManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (EnemyBullet* projectile : projectiles)
	{
		projectile->Render(dc,shader);
	}
}

// 弾丸登録
void EnemyBulletManager::Register(EnemyBullet* projectile)
{
	projectiles.push_back(projectile);
}
// 弾丸全削除
void EnemyBulletManager::Clear()
{
	for (EnemyBullet* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

// 弾丸削除
void EnemyBulletManager::Remove(EnemyBullet* projectile)
{
	// 破棄リストに追加
	removes.emplace_back(projectile);
}
