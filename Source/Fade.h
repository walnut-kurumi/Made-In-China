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

	//�t�F�[�h
	void FadeIn(float speed);
	void FadeOut(float speed);

	// ���Z�b�g
	void ResetFade();

	// �t�F�[�h�t���O�擾
	bool GetFadeInFlag() { return fadeInFlag; }
	bool GetFadeOutFlag() { return fadeOutFlag; }

private:

	std::unique_ptr<Sprite> fade = nullptr;

	bool fadeInFlag = false;
	bool fadeOutFlag = false;
	float fadeAlpha = 0.0f;
};
#pragma once
