#pragma once

#include "Scene.h"
#include "Player.h"
#include "Model.h"
#include "framebuffer.h"
#include "RadialBlur.h"

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

	// 敵の初期化
	void EnemyInitialize(ID3D11Device* device);
	// 敵の座標セット
	void EnemyStatusSetting();
	// 敵の攻撃予兆
	void RenderEnemyAttack();

	// チュートリアル画像描画
	void RenderTutorial();

private:
	std::unique_ptr<Player> player{};


	static const int ENEMY_MAX = 3;
	Vec2 enemyPos[ENEMY_MAX] = {};
	int enemyGroup[ENEMY_MAX] = {};
	bool enemyWalk[ENEMY_MAX] = {};
	bool enemyDirection[ENEMY_MAX] = {};



	Sprite* cursorSprite{};
	Sprite* Bar{};
	Sprite* LoadBar{};
	Sprite* enemyattack{};
	Sprite* fade{};
	//操作説明
	Sprite* KeyA{};
	Sprite* KeyD{};
	Sprite* KeySPACE{};
	Sprite* KeySHIFT{};
	Sprite* LeftClick{};
	Sprite* RightClick{};

	Sprite* ButtonA{};
	Sprite* ButtonX{};
	Sprite* Stick{};
	Sprite* ButtonLT{};
	Sprite* ButtonRT{};	

	// Sprite用変数
	float radian = 0.0f;
	int tutorialTick = 0;	// アニメーション用チック	
	int stickAnim = 0;	// アニメーション	
	bool isKeybord = false;
	float tick = 0.0f;

	bool renderSlow = false;
	bool renderAttack = false;
	bool renderMove = false;
	bool renderJump = false;
	bool renderSB = false;


	Effect* hitEffect = nullptr;
	Effekseer::Handle handle = 0;
	bool a = false;
	int ti = 0;

	// CAMERA_SHAKE
	// https://www.gdcvault.com/play/1023557/Math-for-Game-Programmers-Juicing
	// TODO:01 Define a constant buffer object.
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	/*Microsoft::WRL::ComPtr<ID3D11Buffer> postConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> bloomConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mistConstantBuffer;*/
	// TODO:06 Defines the maximum amount of rotation(max_skew) and movement(max_sway) of the camera.
	float max_skew = 5.00f;		// unit is degrees angle.
	float max_sway = 16.00f;	// unit is pixels in screen space.
	// TODO:11 Define the perlin noise generation object and the seed value.
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
	float sigma = 0.16f;
	float intensity = 0.09f;
	float exp = 1.1f;
	DirectX::XMFLOAT3 LErgb = { 0.2f,0.6f,0.5f };
	std::unique_ptr<Framebuffer> framebuffer[8];
	std::unique_ptr<RadialBlur> radialBlur;
	std::unique_ptr<FullScreenQuad> fullScreen;

	ConstantBuffer<Radial_Blur> CBBlur;

	std::unique_ptr<AudioSource> BGM;

	// チュートリアル用変数
	// チュートリアル中か
	bool isTutorial = true;
	// 時間とめるか
	bool isPause = false;
	// 最初の敵倒したらカメラのターゲットをプレイヤーに変更
	bool cameraTargetChange = false;
	Vec3 camTargetPos = { -19,0,0 };
	// スローにしたか
	bool isSlow = false;


	// 次のシーンへ移る
	bool changeScene = false;
};
