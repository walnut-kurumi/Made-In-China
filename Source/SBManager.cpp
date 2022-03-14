#include "SBManager.h"

SBManager::~SBManager() {
	Clear();
}

// �X�V����
void SBManager::Update(float elapsedTime) {
	// �X�V����
	for (SB* projectile : projectiles) {
		projectile->Update(elapsedTime);
	}
	// �j������
	// ��projectiles�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (SB* projectile : removes) {
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<SB*>::iterator it =
			std::find(projectiles.begin(), projectiles.end(), projectile);
		if (it != projectiles.end()) {
			projectiles.erase(it);
		}
		// �e�ۂ̔j������
		if (projectile) {
			projectile = nullptr;
			delete projectile;
		}
	}
	// �j�����X�g���N���A
	removes.clear();
}
// �`�揈��
void SBManager::Render(ID3D11DeviceContext* dc, Shader* shader) {
	for (SB* projectile : projectiles)	{
		projectile->Render(dc, shader);
	}
}

// �e�ۓo�^
void SBManager::Register(SB* projectile) {
	projectiles.push_back(projectile);
}
// �e�ۑS�폜
void SBManager::Clear() {
	for (SB* projectile : projectiles)	{
		delete projectile;
	}
	projectiles.clear();
}

// �e�ۍ폜
void SBManager::Remove(SB* projectile) {
	// �j�����X�g�ɒǉ�
	removes.emplace_back(projectile);
}
