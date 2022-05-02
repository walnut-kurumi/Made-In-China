#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"


class Fade
{
private:
	Fade();
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

	// �t�F�[�h�t���O�擾
	bool GetFadeInFlag() { return fadeInFlag; }
	void SetFadeInFlag(bool fade) { fadeInFlag = fade; }
	bool GetFadeOutFlag() { return fadeOutFlag; }
	void SetFadeOutFlag(bool fade) { fadeOutFlag = fade; }

	// �t�F�[�h�I�������
	bool GetFadeInFinish() { return fadeInFinish; }	
	bool GetFadeOutFinish() { return fadeOutFinish; }	
	// �t�F�[�h�p�A���t�@�ݒ�
	void SetFadeAlpha(float alpha) { fadeAlpha = alpha; }

private:

	std::unique_ptr<Sprite> fade = nullptr;

	bool fadeInFlag = false;
	bool fadeOutFlag = false;
	bool fadeInFinish = false;
	bool fadeOutFinish = false;
	float fadeAlpha = 0.0f;
};
#pragma once
