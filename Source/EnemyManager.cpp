
#include "EnemyManager.h"
#include "Graphics/Graphics.h"
#include "Collision.h"
#include <algorithm>


// ���f���ǂݍ���
void EnemyManager::ModelLoading(ID3D11Device* device)
{
	const char* idle = "Data/Models/Enemy/Animations/Idle.fbx";
	const char* run = "Data/Models/Enemy/Animations/Running.fbx";
	const char* walk = "Data/Models/Enemy/Animations/Walking.fbx";
	const char* attack = "Data/Models/Enemy/Animations/Attack.fbx";
	const char* blow = "Data/Models/Enemy/Animations/GetHit1.fbx";
	const char* death = "Data/Models/Enemy/Animations/Death.fbx";



	model = new Model(device, "Data/Models/Enemy/Jummo.fbx", true, 0);

	model->LoadAnimation(idle, 0, static_cast<int>(State::Idle));
	model->LoadAnimation(run, 0, static_cast<int>(State::Run));
	model->LoadAnimation(walk, 0, static_cast<int>(State::Walk));
	model->LoadAnimation(attack, 0, static_cast<int>(State::Attack));
	model->LoadAnimation(blow, 0, static_cast<int>(State::Blow));
	model->LoadAnimation(death, 0, static_cast<int>(State::Death));	
	
}

// ����������
void EnemyManager::Init()
{
	for (Enemy* enemy : enemies)
	{
		enemy->SetModel(model);
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

// �G�l�~�[���m�̏Փˏ���
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
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				outPosition))
			{
				enemyB->SetPosition(outPosition);
			}
			if (Collision::IntersectCylinderVsCylinder(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyA->GetHeight(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				enemyB->GetHeight(),
				outPosition))
			{
				enemyB->SetPosition(outPosition);
			}
		}
	}*/
}
