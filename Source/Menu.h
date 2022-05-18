#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "audio/AudioResource.h"
#include "audio/Audio.h"

class Menu
{
private:
	Menu() {};
	~Menu() {};

public:
	// �B��̃C���X�^���X�擾
	static Menu& Instance()
	{
		static Menu instance;
		return instance;
	}

	// ������
	void Initialize();

	// �I����
	void Finalize();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(float elapsedTime);		

	//���j���[
	void menu();

	void SceneSelect();

	// ���j���[�t���O�擾
	bool GetMenuFlag() { return menuflag; }

private:
	
	bool menuflag = false;

	bool retry = false;
	bool end = false;

	float retryAlpha = 1.0f;
	float endAlpha = 1.0f;

	Sprite* gameRetry{};
	Sprite* gameEnd{};

	DirectX::XMFLOAT2 retrysize = { 320,180 };
	DirectX::XMFLOAT2 endsize = { 320,180 };

	DirectX::XMFLOAT2 retrypos = { Graphics::Ins().GetScreenWidth() / 2 - retrysize.x / 2,Graphics::Ins().GetScreenHeight() / 2 - retrysize.y / 2 + 60 };
	DirectX::XMFLOAT2 endpos = { retrypos.x,retrypos.y + endsize.y * 1.2f };

	DirectX::XMFLOAT2 mousepos = {};

	std::unique_ptr<AudioSource> SEMenu;
	std::unique_ptr<AudioSource> SEDecision;
	std::unique_ptr<AudioSource> SECancellation;
};
