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
 
class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override {};

	// ‰Šú‰»
	virtual void Initialize() override;

	// I—¹‰»
	virtual void Finalize() override;

	// XVˆ—
	virtual void Update(float elapsedTime) override;

	// •`‰æˆ—
	virtual void Render(float elapsedTime) override;

	// ƒQ[ƒ€ƒŠƒZƒbƒg
	void Reset();

	// “G‚Ì‰Šú‰»
	void EnemyInitialize(ID3D11Device* device);
	// “G‚ÌÀ•WƒZƒbƒg
	void EnemyPositionSetting();
	// “G‚ÌUŒ‚—\’›
	void RenderEnemyAttack();

private:	
	std::unique_ptr<Player> player{};


	static const int ENEMY_MAX = 3;
	Vec2 enemyPos[ENEMY_MAX] = {};
	int enemyGroup[ENEMY_MAX] = {};
	bool enemyWalk[ENEMY_MAX] = {};

	Sprite* Bar{};
	Sprite* LoadBar{};
	Sprite* enemyattack{};

	ConstantBuffer<scene_blur> SBBlur;
	float sigma = 1.0f;
	float intensity = 0.07f;
	float exp = 1.0f;

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
	Shader BluShader;

	std::unique_ptr<Framebuffer> framebuffer[8];
	std::unique_ptr<RadialBlur> radialBlur;

	ConstantBuffer<Radial_Blur> CBBlur;


};
