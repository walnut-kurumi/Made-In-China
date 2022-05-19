#pragma once

#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Graphics.h"
#include "Player.h"
#include "Input/Input.h"
#include "audio/AudioResource.h"
#include "audio/Audio.h"


// �^�C�g���V�[��
class SceneClear : public Scene
{
public:
	SceneClear() {}
	~SceneClear() override {}

	// ������
	virtual void Initialize() override;

	// �I����
	virtual void Finalize() override;

	// �X�V����
	virtual void Update(float elapsedTime) override;

	// �`�揈��
	virtual void Render(float elapsedTime) override;

	void SceneSelect();

private:
	Sprite* clearSprite{};
	Sprite* cursorSprite{};
	Sprite* gameStart{};
	Sprite* gameEnd{};

	bool start = false;
	bool end = false;

	float startAlpha = 1.0f;
	float endAlpha = 1.0f;

	DirectX::XMFLOAT2 startsize = { 320,180 };
	DirectX::XMFLOAT2 endsize = { 320,180 };

	DirectX::XMFLOAT2 startpos = { Graphics::Ins().GetScreenWidth() / 2 - startsize.x / 2,Graphics::Ins().GetScreenHeight() / 2 - startsize.y / 2 + 60 };
	DirectX::XMFLOAT2 endpos = { startpos.x,startpos.y + endsize.y * 1.2f };

	DirectX::XMFLOAT2 mousepos = {};

	std::unique_ptr<AudioSource> SEDecision;
};
