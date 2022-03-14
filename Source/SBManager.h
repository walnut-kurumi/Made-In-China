#pragma once

#include <vector>
#include "SB.h"

// �e�ۃ}�l�[�W���[
class SBManager
{
public:
	SBManager() {};
	~SBManager();

	// �B��̃C���X�^���X�擾
	static SBManager& Instance()
	{
		static SBManager instance;
		return instance;
	}

	// �X�V����
	void Update(float elapsedTime);
	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	// �e�ۓo�^
	void Register(SB* projectile);
	// �e�ۑS�폜
	void Clear();
	// �e�ې��擾
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }
	// �e�ێ擾
	SB* GetProjectile(int index) { return projectiles.at(index); }
	// �e�ۍ폜
	void Remove(SB* projectile);

private:
	std::vector<SB*>	projectiles;
	std::vector<SB*>	removes;

};
