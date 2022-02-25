#pragma once

#include "Scene.h"
#include "Player.h"
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Model.h"
#include "Framebuffer.h"
#include "FullscreenQuad.h"

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

private:	
	Player* player{};

	Model* model = nullptr;


	SkinnedMesh* sk[8];

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

};
