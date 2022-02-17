#pragma once

#include "Scene.h"
#include <tchar.h>
#include <sstream>
#include "Graphics/Sprite.h"

// ���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene);
	SceneLoading() {}
	~SceneLoading() override {}

	// ������
	virtual void Initialize() override;

	// �I����
	virtual void Finalize() override;

	// �X�V����
	virtual void Update(float elapsedTime) override;

	// �`�揈��
	virtual void Render(float elapsedTime) override;

private:
	// ���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);

private:
	float angle = 0.0f;
	Scene* nextScene = nullptr;
	
	Sprite* loadSprite{};
};