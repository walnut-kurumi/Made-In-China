
#include "EnemyManager.h"
#include "Graphics/Graphics.h"
#include "Collision.h"
#include <algorithm>
#include "StageManager.h"
#include "EnemyBulletManager.h"

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
	//CollisionEnemyVsEnemies();

	// グループ化
	GroupAttack();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* dc, Shader* shader, bool slow)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(dc, shader, slow);
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
	
	enemies.erase(enemies.begin(), enemies.end());
	enemies.shrink_to_fit();
	
	removes.erase(removes.begin(), removes.end());
	removes.shrink_to_fit();
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


void EnemyManager::SetPlayer(Player* p)
{
	for (Enemy* enemy : enemies)
	{
		enemy->SetPlayer(p);		
	}
}


void EnemyManager::SetPosition(int i, DirectX::XMFLOAT3 enemyPos, int gruop,bool walk)
{	
	enemies[i]->SetPosition(enemyPos);
	enemies[i]->SetGroupNum(gruop);
	enemies[i]->SetWalkFlag(walk);
}

void EnemyManager::SetPosition(int i, DirectX::XMFLOAT3 enemyPos, bool walk)
{
	enemies[i]->SetPosition(enemyPos);
	enemies[i]->SetWalkFlag(walk);
}

// エネミーすべてリセット
void EnemyManager::EnemyReset()
{
	for (Enemy* enemy : enemies)
	{
		enemy->PositionInitialize();
		enemy->GroupNumInitialize();
		enemy->WalkFlagInitialize();
		enemy->DirectionInitialize();
	}
}

void EnemyManager::EnemyKill(int i)
{	
	enemies.at(i)->SetHealth(0);
	enemies.at(i)->OnDead();
	enemies.at(i)->Kill(true);
}

// グループで攻撃してくる
void EnemyManager::GroupAttack()
{
	for (Enemy* enemy : enemies)
	{
		//生きてる && 自機見つけている
		if (enemy->GetHealth() > 0 && enemy->GetIsSearch())
		{
			for (Enemy* enemy2 : enemies)
			{
				// 同じグループなら
				if (enemy->GetGroupNum() == enemy2->GetGroupNum())
				{
					// 見つける
					enemy2->SetIsSearch(true);
				}
			}
		}
		// 死んでる
		else if (enemy->GetHealth() <= 0)
		{
			for (Enemy* enemy2 : enemies)
			{
				// 同じグループなら
				if (enemy->GetGroupNum() == enemy2->GetGroupNum())
				{
					// 見つける
					enemy2->SetIsSearch(true);
				}
			}
		}
	}
}

// 死んでるエネミーの数
int EnemyManager::GetDeadEnemyCount()
{	
	int dec = 0;
	for (Enemy* enemy : enemies)
	{
		if (enemy->GetHealth() <= 0)dec++;
	}
	return dec;
}


// エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEnemies()
{
	size_t enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{		
		Enemy* enemyA = enemies.at(i);
		if (enemyA->GetHealth() <= 0)break;

		for (int j = i + 1; j < enemyCount; ++j)
		{			
			Enemy* enemyB = enemies.at(j);


			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				outPosition))
			{								
				outPosition.z = 0;

				// 下レイキャスト
				{
					float my = outPosition.y - enemyB->GetPosition().y;

					// レイの開始位置は足元より少し上
					DirectX::XMFLOAT3 start = { outPosition.x, outPosition.y + enemyB->GetStepOffset(), outPosition.z };
					// レイの終点位置は移動後の位置
					DirectX::XMFLOAT3 end = { outPosition.x, outPosition.y + my, outPosition.z };

					// レイキャストによる地面判定
					HitResult hit;
					if (StageManager::Instance().RayCast(start, end, hit)) {

						// 地面に接地している
						outPosition.x = hit.position.x;
						outPosition.y = hit.position.y;
						outPosition.z = hit.position.z;
					}
					else {
						// 空中に浮いてる
						outPosition.y = enemyB->GetPosition().y;
					}
				}
				// 横レイキャスト
				{
					float mx = outPosition.x - enemyB->GetPosition().x;

					// レイの開始位置は足元より少し上
					DirectX::XMFLOAT3 start = { outPosition.x, outPosition.y + enemyB->GetStepOffset(), outPosition.z };
					// レイの終点位置は移動後の位置
					DirectX::XMFLOAT3 end = { outPosition.x + mx, outPosition.y + enemyB->GetStepOffset(), outPosition.z };

					// レイキャストによる地面判定
					HitResult hit;
					if (StageManager::Instance().RayCast(start, end, hit)) {

						// 地面に接地している
						outPosition.x = hit.position.x;						
						outPosition.z = hit.position.z;
					}					
				}

				enemyB->SetPosition(outPosition);
			}			
		}
	}
}
