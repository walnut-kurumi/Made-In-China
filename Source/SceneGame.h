#pragma once

#include "Scene.h"
#include "Player.h"
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Model.h"
#include "Framebuffer.h"
#include "FullscreenQuad.h"

class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override {};

	// ������
	virtual void Initialize() override;

	// �I����
	virtual void Finalize() override;

	// �X�V����
	virtual void Update(float elapsedTime) override;

	// �`�揈��
	virtual void Render(float elapsedTime) override;

	// �Q�[�����Z�b�g
	void Reset();

	void EnemyPositionSetting();

private:	
	Player* player{};

	Model* model = nullptr;

	Vec2 enemyPos[10] = {};

	SkinnedMesh* sk[8];
};
