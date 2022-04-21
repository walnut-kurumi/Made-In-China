
#include "EnemyManager.h"
#include "Graphics/Graphics.h"
#include "Collision.h"
#include <algorithm>


// ����������
void EnemyManager::Init()
{
	for (Enemy* enemy : enemies)
	{	
		enemy->Init();
	}
}

// �X�V����
void EnemyManager::Update(float elapsedTime)
{
	
	// �X�V����
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	// �j������
	// ��enemies�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Enemy* enemy : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}
		// �폜
		delete enemy;
	}
	// �j�����X�g���N���A
	removes.clear();

	// �G���m�̏Փˏ���
	CollisionEnemyVsEnemies();

	// �O���[�v��
	GroupAttack();
}

// �`�揈��
void EnemyManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(dc, shader);
	}
}


// �G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

// �G�l�~�[�폜
void EnemyManager::Remove(Enemy* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.emplace_back(enemy);
}

// �G�l�~�[�S�폜
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

// �\�[�g
void EnemyManager::SortLengthSq(const DirectX::XMFLOAT3 playerPos)
{
	// �e�G�Ƀv���C���[�Ƃ̋�����2���ێ�����
	for (auto& enemy : enemies)
	{
		float vx = playerPos.x - enemy->GetPosition().x;
		float vy = playerPos.y - enemy->GetPosition().y;
		float vz = playerPos.z - enemy->GetPosition().z;
		enemy->SetLengthSq(vx * vx + vy * vy + vz * vz);
	}
	// �v���C���[�Ƃ̋�����2��Ń\�[�g����
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

// �O���[�v�ōU�����Ă���
void EnemyManager::GroupAttack()
{
	for (Enemy* enemy : enemies)
	{
		//�����Ă� && ���@�����Ă���
		if (enemy->GetHealth() > 0 && enemy->GetIsSearch())
		{
			for (Enemy* enemy2 : enemies)
			{
				// �����O���[�v�Ȃ�
				if (enemy->GetGroupNum() == enemy2->GetGroupNum())
				{
					// ������
					enemy2->SetIsSearch(true);
				}
			}

		}
	}
}

// ����ł�G�l�~�[�̐�
int EnemyManager::GetDeadEnemyCount()
{	
	int dec = 0;
	for (Enemy* enemy : enemies)
	{
		if (enemy->GetHealth() <= 0)dec++;
	}
	return dec;
}


// �G�l�~�[���m�̏Փˏ���
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
				enemyB->SetPosition(outPosition);
			}			
		}
	}
}
