#pragma once

#include "Scene.h"
#include "Player.h"
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Model.h"
#include "framebuffer.h"
#include "RadialBlur.h"

class SceneTutorial : public Scene
{
public:
	SceneTutorial() {};
	~SceneTutorial() override {};

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

	// 敵の座標セット
	void EnemyPositionSetting();

	void RenderEnemyAttack();

private:
	// プレイヤー
	std::unique_ptr<Player> player{};
	
	// エネミー
	static const int ENEMY_MAX = 3;
	Vec2 enemyPos[ENEMY_MAX] = {};

	// スプライト
	Sprite* Bar{};
	Sprite* LoadBar{};
	Sprite* enemyattack{};

	float seed = 0;
	float seed_shifting_factor = 3.00f;

	float w = 0.0f;
	float et = 0;

	bool menuflag = false;

	DirectX::XMFLOAT2 mousepos = {};

	//シェーダー用の変数
	Shader BluShader;
	ConstantBuffer<scene_blur> SBBlur;
	ConstantBuffer<Radial_Blur> CBBlur;
	std::unique_ptr<RadialBlur> radialBlur;
	std::unique_ptr<Framebuffer> framebuffer[8];
	float sigma = 1.0f;
	float intensity = 0.07f;
	float exp = 1.0f;

};
