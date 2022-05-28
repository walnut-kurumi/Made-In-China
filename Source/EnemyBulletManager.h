#pragma once

#include <vector>
#include "EnemyBullet.h"

// ’eŠÛƒ}ƒl[ƒWƒƒ[
class EnemyBulletManager
{
public:
	EnemyBulletManager() {};
	~EnemyBulletManager();

	// —Bˆê‚ÌƒCƒ“ƒXƒ^ƒ“ƒXæ“¾
	static EnemyBulletManager& Instance()
	{
		static EnemyBulletManager instance;
		return instance;
	}

	// XVˆ—
	void Update(float elapsedTime);
	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc,Shader* shader, bool slow);
	// ’eŠÛ“o˜^
	void Register(EnemyBullet* projectile);
	// ’eŠÛ‘Síœ
	void Clear();
	// ’eŠÛ”æ“¾
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }
	// ’eŠÛæ“¾
	EnemyBullet* GetProjectile(int index) { return projectiles.at(index); }
	// ’eŠÛíœ
	void Remove(EnemyBullet* projectile);

private:
	std::vector<EnemyBullet*>	projectiles;
	std::vector<EnemyBullet*>	removes;

};
