#pragma once

#include "Graphics/Shader.h"
#include "Character.h"
#include "DebugRenderer.h"
#include "EnemyBulletManager.h"
#include "Player.h"
#include "Effect.h"
#include "DoorManager.h"

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

	// �������W�Z�b�g
	void SetInitialPos(Vec3 pos) { initialPos = pos; }
	// ���W������
	void PositionInitialize() { position = initialPos; }

	// �O���[�v�ԍ��Z�b�g
	void SetGroupNum(int g) { groupNum = g; }
	// �O���[�v�ԍ��Q�b�g
	int GetGroupNum() { return groupNum; }
	// �������p
	void SetInitialGroupNum(int g) { initialGroupNum = g; }
	// ������
	void GroupNumInitialize() { groupNum = initialGroupNum; }


	// �v���C���[�������Ă��邩�̃t���O�Q�b�g
	void SetIsSearch(bool s) { isSearch = s; }
	// �v���C���[�������Ă��邩�̃t���O�Q�b�g
	bool GetIsSearch() { return isSearch; }

	// �p�j���邩�ǂ���
	void SetWalkFlag(bool b) { walk = b; }
	// �������p
	void SetInitialWalk(bool b) { initialWalk = b; }
	// ������
	void WalkFlagInitialize() { walk = initialWalk; }	

	// �ŏ��̌��� ����true�@false�E��
	void SetInitialDirection(bool b) { initialDirection = b; }
	void DirectionInitialize() { direction = initialDirection; }

	// ���׼�
	void FallIsDead();

protected:
	// ���������Ԃ̃O���[�v��
	int groupNum = 0;
	int initialGroupNum = 0;	

	// �v���C���[�������Ă��邩
	bool isSearch = false;
	// ������邩�ǂ���
	bool walk = false;
	bool initialWalk = false;


	// �U���\��Flag
	bool isAttack = false;

	// ����ł邩
	bool isDead = false;
	
	// �ړ�������� ����true�@false�E��
	bool initialDirection = false;
	bool direction = false;


	// �������W
	Vec3 initialPos = {};

	float lengthSq = 0;

	// �v���C���[
	Player* player = nullptr;

	// �f�o�b�O
	std::unique_ptr<DebugRenderer> debugRenderer;

	// �}�e���A���J���[
	Vec4 materialColor = { 1,1,1,1 };


	// �G�t�F�N�g
	Effect* deadEffect = nullptr;	
	Effekseer::Handle handle = 0;
};