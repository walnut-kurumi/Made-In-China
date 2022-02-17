#pragma once

#include "Scene.h"
#include <mutex>

// シーンマネージャー
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

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(float elapsedTime);

	// クリア
	void Clear();

	// シーン切り替え
	void ChangeScene(Scene* scene);

	void TimerAdd(float elapsedTime) { timer += elapsedTime; }
	const float& GetTimet() const { return timer; }
	void TimerClear() { timer = 0.0f; }

	//マルチスレッド用まだつかわにゃい
	std::mutex& GetMutex() { return mutex; }

private:
	Scene* currentScene{}; // 現在のシーン

	std::mutex mutex;

	float timer = 0.0f;
};
