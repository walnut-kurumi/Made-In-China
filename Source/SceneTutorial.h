#pragma once

#include "Scene.h"
#include "Player.h"
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Model.h"

class SceneTutorial : public Scene
{
public:
	SceneTutorial() {};
	~SceneTutorial() override {};

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

	// �G�̍��W�Z�b�g
	void EnemyPositionSetting();

	void RenderEnemyAttack();

private:
	// �v���C���[
	std::unique_ptr<Player> player{};
	
	// �G�l�~�[
	static const int ENEMY_MAX = 3;
	Vec2 enemyPos[ENEMY_MAX] = {};

	// �X�v���C�g
	Sprite* Bar{};
	Sprite* LoadBar{};
	Sprite* enemyattack{};

	float seed = 0;
	float seed_shifting_factor = 3.00f;

	float w = 0.0f;
	float et = 0;

	bool menuflag = false;

	DirectX::XMFLOAT2 mousepos = {};
};