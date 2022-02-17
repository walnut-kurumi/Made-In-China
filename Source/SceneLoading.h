#pragma once

#include "Scene.h"
#include <tchar.h>
#include <sstream>
#include "Graphics/Sprite.h"

// ローディングシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene);
	SceneLoading() {}
	~SceneLoading() override {}

	// 初期化
	virtual void Initialize() override;

	// 終了化
	virtual void Finalize() override;

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	virtual void Render(float elapsedTime) override;

private:
	// ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:
	float angle = 0.0f;
	Scene* nextScene = nullptr;
	
	Sprite* loadSprite{};
};