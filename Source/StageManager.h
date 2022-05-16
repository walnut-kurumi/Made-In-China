#pragma once

#include "Stage.h"
#include "Collision.h"

// �R�s�[�R���X�g���N�^�h�~�}�N��
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

class StageManager
{
private:
	StageManager() = default;
	~StageManager() = default;
	static StageManager* instance;

public:
	// �N���X��private:�ɒu�����ƂŃf�t�H���g�̃R�s�[�R���X�g���N�^
	// ������Z�q�𐶐������Ȃ��}�N�� Readable code����B
	DISALLOW_COPY_AND_ASSIGN(StageManager);

	static StageManager& Instance()	{
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new StageManager;
	}

	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	void Init();

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc, float elapsedTime);

	void RenderGui();

	void Register(Stage* stage);

	void Clear();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	int GetStageCount()const { return static_cast<int>(stages.size()); }
	Stage* GetStage(int index) { return stages.at(index); }

	// �v���C���[�Ǝ��̃X�e�[�W�ֈڂ�Ƃ��̏ꏊ�Ŕ���
	bool CollisionPlayerVsNextStagePos(Vec3 playerPos, float playerRadius);

private:
	std::vector<Stage*> stages;

};