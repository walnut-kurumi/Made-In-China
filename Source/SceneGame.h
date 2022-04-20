#pragma once

#include "Scene.h"
#include "Player.h"
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Model.h"

#include <directxmath.h>
#include "PerlinNoise.h"
#include"Graphics/Misc.h"


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

	// 敵の座標セット
	void EnemyPositionSetting();

	//メニュー
	void menu();

	void SceneSelect();

	void RenderEnemyAttack();

private:	
	Player* player{};

	Vec2 enemyPos[9] = {};

	Sprite* Bar{};
	Sprite* LoadBar{};
	Sprite* enemyattack{};


	// CAMERA_SHAKE
	// https://www.gdcvault.com/play/1023557/Math-for-Game-Programmers-Juicing
	// TODO:01 Define a constant buffer object.
	ID3D11Buffer* constant_buffer;
	// TODO:06 Defines the maximum amount of rotation(max_skew) and movement(max_sway) of the camera.
	float max_skew = 5.00f;		// unit is degrees angle.
	float max_sway = 16.00f;	// unit is pixels in screen space.
	// TODO:11 Define the perlin noise generation object and the seed value.
	// https://solarianprogrammer.com/2012/07/18/perlin-noise-cpp-11/
	PerlinNoise pn;
	float seed = 0;
	float seed_shifting_factor = 3.00f;

	float slowMaxTime = 10.0f;
	float w = 0.0f;
	float et = 0;

	bool menuflag = false;

	bool start = false;
	bool end = false;

	float startAlpha = 1.0f;
	float endAlpha = 1.0f;

	Sprite* gameStart{};
	Sprite* gameEnd{};

	DirectX::XMFLOAT2 startsize = { 320,180 };
	DirectX::XMFLOAT2 endsize = { 320,180 };

	DirectX::XMFLOAT2 startpos = { Graphics::Ins().GetScreenWidth() / 2 - startsize.x / 2,Graphics::Ins().GetScreenHeight() / 2 - startsize.y / 2 + 60 };
	DirectX::XMFLOAT2 endpos = { startpos.x,startpos.y + endsize.y * 1.2f };

	DirectX::XMFLOAT2 mousepos = {};

};
