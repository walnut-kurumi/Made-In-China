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

	// �G�̏�����
	void EnemyInitialize(ID3D11Device* device);
	// �G�̍��W�Z�b�g
	void EnemyPositionSetting();
	// �G�̍U���\��
	void RenderEnemyAttack();

private:	
	std::unique_ptr<Player> player{};


	static const int ENEMY_MAX = 9;
	Vec2 enemyPos[ENEMY_MAX] = {};

	Sprite* Bar{};
	Sprite* LoadBar{};
	Sprite* enemyattack{};

	/*ConstantBuffer<POST_EFFECT_CONSTANTS> postEffectConstant;
	ConstantBuffer<BLOOM_CONSTANTS> bloomConstant;
	ConstantBuffer<MIST_CONSTANTS> mistConstant;
	enum class FRAMEBUFFER { SCENE_MSAA, SCENE_RESOLVED, POST_PROCESSED };
	std::unique_ptr<framebuffer> framebuffers[8];
	std::unique_ptr<fullscreen_quad> bitBlockTransfer;

	enum class TEXTURE { T0, WAVE, T2, T3, ENVIRONMENT, DISTORTION };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[8];

	PixelShader postEffectPs;
	PixelShader toonMapPs;

	std::unique_ptr<bloom> bloom_effect;

	std::unique_ptr<Rasterizer> rasterizer;
	std::unique_ptr<Blender> blender;
	std::unique_ptr<DeppthSteciler> depth;*/

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
};
