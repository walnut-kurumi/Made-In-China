#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"


class Menu
{
private:
	Menu() {};
	~Menu() {};

public:
	// 唯一のインスタンス取得
	static Menu& Instance()
	{
		static Menu instance;
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
	void menu();

	void SceneSelect();

	// メニューフラグ取得
	bool GetMenuFlag() { return menuflag; }

private:
	
	bool menuflag = false;

	bool start = false;
	bool end = false;

	float startAlpha = 1.0f;
	float endAlpha = 1.0f;

	Sprite* gameStart{};
	Sprite* gameEnd{};

	DirectX::XMFLOAT2 startsize = { 320,180 };
	DirectX::XMFLOAT2 endsize = { 320,180 };

	DirectX::XMFLOAT2 startpos = { Graphics::Ins().GetScreenWidth() / 2 - startsize.x / 2,Graphics::Ins().GetScreenHeight() / 2 - startsize.y / 2 + 60 };
	DirectX::XMFLOAT2 endpos = { startpos.x,startpos.y + endsize.y * 1.2f };

	DirectX::XMFLOAT2 mousepos = {};

};
