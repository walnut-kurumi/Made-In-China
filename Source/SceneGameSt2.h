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

class SceneGameSt2 : public Scene
{
public:
	SceneGameSt2() {};
	~SceneGameSt2() override {};

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

	// �G�̏�����
	void EnemyInitialize(ID3D11Device* device);
	// �G�̍��W�Z�b�g
	void EnemyPositionSetting();
	// �G�̍U���\��
	void RenderEnemyAttack();

private:
	std::unique_ptr<Player> player{};


	static const int ENEMY_MAX = 7;
	Vec2 enemyPos[ENEMY_MAX] = {};
	int enemyGroup[ENEMY_MAX] = {};
	bool enemyWalk[ENEMY_MAX] = {};
	bool enemyDirection[ENEMY_MAX] = {};

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

	//�V�F�[�_�[�p�̕ϐ�
	Shader BluShader;
	ConstantBuffer<scene_blur> SBBlur;
	Shader LumiShader;
	ConstantBuffer<scene_col> LEcol;
	float sigma = 1.0f;
	float intensity = 0.07f;
	float exp = 1.3f;
	DirectX::XMFLOAT3 LErgb = {};
	std::unique_ptr<Framebuffer> framebuffer[8];
	std::unique_ptr<RadialBlur> radialBlur;
	std::unique_ptr<FullScreenQuad> fullScreen;

	ConstantBuffer<Radial_Blur> CBBlur;

	// ���̃V�[���ֈڂ�
	bool changeScene = false;
};