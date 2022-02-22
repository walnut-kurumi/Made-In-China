#pragma once

#include <vector>
#include "EnemyBullet.h"

// �e�ۃ}�l�[�W���[
class EnemyBulletManager
{
public:
	EnemyBulletManager();
	~EnemyBulletManager();
	// �X�V����
	void Update(float elapsedTime);
	// �`�揈��
	void Render(ID3D11DeviceContext* dc,Shader* shader);
	// �e�ۓo�^
	void Register(EnemyBullet* projectile);
	// �e�ۑS�폜
	void Clear();
	// �e�ې��擾
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }
	// �e�ێ擾
	EnemyBullet* GetProjectile(int index) { return projectiles.at(index); }
	// �e�ۍ폜
	void Remove(EnemyBullet* projectile);

private:
	std::vector<EnemyBullet*>	projectiles;
	std::vector<EnemyBullet*>	removes;

};
