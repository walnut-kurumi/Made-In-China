
#include "EnemyManager.h"
#include "Graphics/Graphics.h"
#include "Collision.h"
#include <algorithm>


// 初期化処理
void EnemyManager::Init()
{
	for (Enemy* enemy : enemies)
	{	
		enemy->Init();
	}
}

// 更新処理
void EnemyManager::Update(float elapsedTime)
{
	// 更新処理
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	// 破棄処理
	// ※enemiesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Enemy* enemy : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}
		// 削除
		delete enemy;
	}
	// 破棄リストをクリア
	removes.clear();

	// 敵同士の衝突処理
	CollisionEnemyVsEnemies();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(dc, shader);
	}
}


// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

// エネミー削除
void EnemyManager::Remove(Enemy* enemy)
{
	// 破棄リストに追加
	removes.emplace_back(enemy);
}

// エネミー全削除
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
}

// ソート
void EnemyManager::SortLengthSq(const DirectX::XMFLOAT3 playerPos)
{
	// 各敵にプレイヤーとの距離の2乗を保持する
	for (auto& enemy : enemies)
	{
		float vx = playerPos.x - enemy->GetPosition().x;
		float vy = playerPos.y - enemy->GetPosition().y;
		float vz = playerPos.z - enemy->GetPosition().z;
		enemy->SetLengthSq(vx * vx + vy * vy + vz * vz);
	}
	// プレイヤーとの距離の2乗でソートする
	std::sort(enemies.begin(), enemies.end(),
		[](Enemy* a, Enemy* b)->int {return a->GetLengthSq() < b->GetLengthSq(); });
}


void EnemyManager::SetPlayerPos(Vec2 playerPos)
{	
	for (Enemy* enemy : enemies)
	{
		enemy->SetPlayerPos(playerPos);
	}
}

// エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEnemies()
{
	/*size_t enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemyA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* enemyB = enemies.at(j);
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				enemyA->GetCenterPosition(),
				enemyA->GetRadius(),
				enemyB->GetCenterPosition(),
				enemyB->GetRadius(),
				outPosition))
			{
				enemyB->SetPosition(outPosition);
			}			
		}
	}*/
}
