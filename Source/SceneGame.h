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

	// 初期化
	virtual void Initialize() override;

	// 終了化
	virtual void Finalize() override;

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	virtual void Render(float elapsedTime) override;

	// ゲームリセット
	void Reset();

	void EnemyPositionSetting();

private:	
	Player* player{};

	Model* model = nullptr;

	Vec2 enemyPos[10] = {};

	SkinnedMesh* sk[8];
};
