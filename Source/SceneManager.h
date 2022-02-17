#pragma once

#include "Scene.h"
#include <mutex>

// �V�[���}�l�[�W���[
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	void Init();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(float elapsedTime);

	// �N���A
	void Clear();

	// �V�[���؂�ւ�
	void ChangeScene(Scene* scene);

	void TimerAdd(float elapsedTime) { timer += elapsedTime; }
	const float& GetTimet() const { return timer; }
	void TimerClear() { timer = 0.0f; }

	//�}���`�X���b�h�p�܂�����ɂႢ
	std::mutex& GetMutex() { return mutex; }

private:
	Scene* currentScene{}; // ���݂̃V�[��

	std::mutex mutex;

	float timer = 0.0f;
};
