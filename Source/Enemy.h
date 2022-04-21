#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "EnemyBulletManager.h"
#include "Player.h"

// �G�l�~�[
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}
	
	// ����������
	virtual void Init() = 0;
	// �X�V����
	virtual void Update(float elapsedTime) = 0;
	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;	

	// �j��
	void Destroy();

	void SetLengthSq(float sq) { lengthSq = sq; }
	float GetLengthSq()const { return lengthSq; }

	void SetPlayer(Player* p) { player = p; }
	
	// ���S�������ɌĂ΂��
	void OnDead() override { isDead = true; }
	
	// �U���\�����o�����ǂ���
	bool GetIsAttack() { return isAttack; }

	// �O���[�v�ԍ��Z�b�g
	void SetGroupNum(int g) { groupNum = g; }
	// �O���[�v�ԍ��Q�b�g
	int GetGroupNum() { return groupNum; }

	// �v���C���[�������Ă��邩�̃t���O�Q�b�g
	void SetIsSearch(bool s) { isSearch = s; }
	// �v���C���[�������Ă��邩�̃t���O�Q�b�g
	bool GetIsSearch() { return isSearch; }

	// �p�j���邩�ǂ���
	void SetWalkFlag(bool b) { walk = b; }


protected:
	// ���������Ԃ̃O���[�v��
	int groupNum = 0;
	// �v���C���[�������Ă��邩
	bool isSearch = false;
	// ������邩�ǂ���
	bool walk = false;

	// �U���\��Flag
	bool isAttack = false;

	// ����ł邩
	bool isDead = false;
	

	float lengthSq = 0;

	// �v���C���[
	Player* player = nullptr;

	// �f�o�b�O
	std::unique_ptr<DebugRenderer> debugRenderer;

	// �}�e���A���J���[
	Vec4 materialColor = { 1,1,1,1 };
};