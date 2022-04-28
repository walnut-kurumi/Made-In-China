#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"


class Fade
{
private:
	Fade() {};
	~Fade() {};

public:
	// �B��̃C���X�^���X�擾
	static Fade& Instance()
	{
		static Fade instance;
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
	void FadeIn();
	void FadeOut();

	// �t�F�[�h�t���O�擾
	bool GetFadeInFlag() { return fadeInFlag; }
	bool GetFadeOutFlag() { return fadeOutFlag; }

private:

	std::unique_ptr<Sprite> fade = nullptr;

	bool fadeInFlag = false;
	bool fadeOutFlag = false;
	float fadeAlha = 0.0f;
};
#pragma once
