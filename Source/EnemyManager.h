#pragma once

#include <vector>
#include "Enemy.h"

// �G�l�~�[�}�l�[�W���[
class EnemyManager
{
private:
	enum class State
	{
		Idle,		// �ҋ@ �p�j�^�[�����ɏ�������
		Run,		// �ړG������˒������ɓ���܂�
		Walk,		// �p�j ��{����
		Attack,		// �˒������ɓ������玀�ʂ��˒��������痣���܂ōU��
		Blow,		// �v���_���[�W���󂯂��Ƃ��ɐ�����΂��Ƃ��̎p��
		Death,		// ������΂��ꂽ���Ƃ̎p��
		End,		// ���@��@��
	};

private:
	EnemyManager() {}
	~EnemyManager() { delete model; }
public:
	// �B��̃C���X�^���X�擾
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}	
	// ����������
	void Init();
	// �X�V����
	void Update(float elapsedTime);
	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);	
	// �G�l�~�[�o�^
	void Register(Enemy* enemy);
	// �G�l�~�[�폜
	void Remove(Enemy* enemy);
	// �G�l�~�[�S�폜
	void Clear();	
	// �G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }
	// �G�l�~�[�擾
	Enemy* GetEnemy(int index) { return enemies.at(index); }
	// �\�[�g
	void SortLengthSq(const DirectX::XMFLOAT3 playerPos);

	void SetPlayerPos(Vec2 playerPos);

private:
	// �G�l�~�[���m�̏Փˏ���
	void CollisionEnemyVsEnemies();
	// �����̃G�l�~�[���Ǘ����邽�߁A�|�C���^��vector�ŊǗ�����
	std::vector<Enemy*> enemies;
	std::vector<Enemy*> removes;	

	Model* model = nullptr;
};
