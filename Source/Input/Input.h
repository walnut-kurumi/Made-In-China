#pragma once

#include "GamePad.h"
#include "Mouse.h"
#include "Keyboard.h"

// インプット
class Input
{
public:
	Input(HWND hWnd);
	~Input() {}

public:
	// インスタンス取得
	static Input& Instance() { return *ins; }

	// 更新処理
	void update();

	// ゲームパッド取得
	GamePad& GetGamePad() { return gamePad; }

	// マウス取得
	Mouse& GetMouse() { return mouse; }

	// キーボード取得
	Key& GetKey() { return key; }

private:
	static Input*		ins;
	GamePad				gamePad;
	Mouse				mouse;
	Key                 key;	
};
