#include "EnemyBulletManager.h"



EnemyBulletManager::~EnemyBulletManager()
{
	Clear();
}

// �X�V����
void EnemyBulletManager::Update(float elapsedTime)
{
	// �X�V����
	for (EnemyBullet* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}
	// �j������
	// ��projectiles�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (EnemyBullet* projectile : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<EnemyBullet*>::iterator it = std::find(projectiles.begin(), projectiles.end(),
			projectile);
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}
		// �e�ۂ̔j������
		delete projectile;
	}
	// �j�����X�g���N���A
	removes.clear();
}
// �`�揈��
void EnemyBulletManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (EnemyBullet* projectile : projectiles)
	{
		projectile->Render(dc,shader);
	}
}

// �e�ۓo�^
void EnemyBulletManager::Register(EnemyBullet* projectile)
{
	projectiles.push_back(projectile);
}
// �e�ۑS�폜
void EnemyBulletManager::Clear()
{
	for (EnemyBullet* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

// �e�ۍ폜
void EnemyBulletManager::Remove(EnemyBullet* projectile)
{
	// �j�����X�g�ɒǉ�
	removes.emplace_back(projectile);
}
