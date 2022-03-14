#pragma once

#include <vector>
#include "SB.h"

// ’eŠÛƒ}ƒl[ƒWƒƒ[
class SBManager
{
public:
	SBManager() {};
	~SBManager();

	// —Bˆê‚ÌƒCƒ“ƒXƒ^ƒ“ƒXæ“¾
	static SBManager& Instance()
	{
		static SBManager instance;
		return instance;
	}

	// XVˆ—
	void Update(float elapsedTime);
	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	// ’eŠÛ“o˜^
	void Register(SB* projectile);
	// ’eŠÛ‘Síœ
	void Clear();
	// ’eŠÛ”æ“¾
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }
	// ’eŠÛæ“¾
	SB* GetProjectile(int index) { return projectiles.at(index); }
	// ’eŠÛíœ
	void Remove(SB* projectile);

private:
	std::vector<SB*>	projectiles;
	std::vector<SB*>	removes;

};
