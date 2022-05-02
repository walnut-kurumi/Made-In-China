#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"


class Fade
{
private:
	Fade();
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

	//フェード
	void FadeIn(float speed);
	void FadeOut(float speed);

	// フェードフラグ取得
	bool GetFadeInFlag() { return fadeInFlag; }
	void SetFadeInFlag(bool fade) { fadeInFlag = fade; }
	bool GetFadeOutFlag() { return fadeOutFlag; }
	void SetFadeOutFlag(bool fade) { fadeOutFlag = fade; }

	// フェード終わったか
	bool GetFadeInFinish() { return fadeInFinish; }	
	bool GetFadeOutFinish() { return fadeOutFinish; }	
	// フェード用アルファ設定
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
