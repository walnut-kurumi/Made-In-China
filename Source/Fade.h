#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"


class Fade
{
private:
	Fade() {};
	~Fade() {};

public:
	// 唯一のインスタンス取得
	static Fade& Instance()
	{
		static Fade instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(float elapsedTime);

	//メニュー
	void FadeIn();
	void FadeOut();

	// フェードフラグ取得
	bool GetFadeInFlag() { return fadeInFlag; }
	bool GetFadeOutFlag() { return fadeOutFlag; }

private:

	std::unique_ptr<Sprite> fade = nullptr;

	bool fadeInFlag = false;
	bool fadeOutFlag = false;
	float fadeAlha = 0.0f;
};
#pragma once
