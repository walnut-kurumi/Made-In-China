#include "Input.h"

Input* Input::ins{};

// コンストラクタ
Input::Input(HWND hWnd)
	: mouse(hWnd) {
	ins = this;
}

// 更新処理
void Input::update() {
	gamePad.Update();
	mouse.Update();
}
