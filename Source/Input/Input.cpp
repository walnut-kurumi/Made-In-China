#include "Input.h"

Input* Input::ins{};

// �R���X�g���N�^
Input::Input(HWND hWnd)
	: mouse(hWnd) {
	ins = this;
}

// �X�V����
void Input::update() {
	gamePad.Update();
	mouse.Update();
}
