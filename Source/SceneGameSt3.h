#pragma once

#include "Scene.h"
#include "Player.h"
#include "Model.h"

#include <directxmath.h>
#include "PerlinNoise.h"
#include "Graphics/Misc.h"
#include "Effect.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Shader.h"
#include"Graphics/Rasterizer.h"

#include "framebuffer.h"
#include "RadialBlur.h"
#include "FullScreenQuad.h"
#include "audio/AudioResource.h"
#include "audio/Audio.h"

class SceneGameSt3 : public Scene
{
public:
	SceneGameSt3() {};
	~SceneGameSt3() override {};

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

	// 敵の初期化
	void EnemyInitialize(ID3D11Device* device);
	// 敵の座標セット
	void EnemyStatusSetting();
	// 敵の攻撃予兆
	void RenderEnemyAttack();

	// リセット時の座標更新
	void UpdateResetPos();

private:
	std::unique_ptr<Player> player{};

	// リセット用のポジション
	Vec3 ResetPos{ 0,0,0 };
	bool checkPoint = false;

	static const int ENEMY_MAX = 11;
	Vec2 enemyPos[ENEMY_MAX] = {};
	int enemyGroup[ENEMY_MAX] = {};
	bool enemyWalk[ENEMY_MAX] = {};
	bool enemyDirection[ENEMY_MAX] = {};

	Sprite* cursorSprite{};
	Sprite* Bar{};
	Sprite* LoadBar{};
	Sprite* enemyattack{};


	Effect* hitEffect = nullptr;
	Effekseer::Handle handle = 0;
	bool a = false;
	int ti = 0;

	// CAMERA_SHAKE
	// https://www.gdcvault.com/play/1023557/Math-for-Game-Programmers-Juicing
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	float max_skew = 5.00f;		// unit is degrees angle.
	float max_sway = 16.00f;	// unit is pixels in screen space.
	// https://solarianprogrammer.com/2012/07/18/perlin-noise-cpp-11/
	PerlinNoise pn;
	float seed = 0;
	float seed_shifting_factor = 3.00f;

	float w = 0.0f;
	float et = 0;

	bool menuflag = false;

	DirectX::XMFLOAT2 mousepos = {};

	//シェーダー用の変数
	Shader BluShader;
	ConstantBuffer<scene_blur> SBBlur;
	Shader LumiShader;
	ConstantBuffer<scene_col> LEcol;
	float sigma = 0.7f;
	float intensity = 0.127f;
	float exp = 1.5f;
	DirectX::XMFLOAT3 LErgb = { 0.4f,0.6f,0.2f };
	std::unique_ptr<Framebuffer> framebuffer[8];
	std::unique_ptr<RadialBlur> radialBlur;
	std::unique_ptr<FullScreenQuad> fullScreen;

	ConstantBuffer<Radial_Blur> CBBlur;
	std::unique_ptr<AudioSource> BGM;

	// 次のシーンへ移る
	bool changeScene = false;

	Sprite* fade{};
};