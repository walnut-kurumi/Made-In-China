#include "Mouse.h"
#include "Input.h"

static const int keyMap[] = {
	VK_LBUTTON,		// ���{�^��
	VK_MBUTTON,		// ���{�^��
	VK_RBUTTON,		// �E�{�^��
};

// �R���X�g���N�^
Mouse::Mouse(HWND hWnd)
	: hWnd(hWnd) {
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;


	::SetCursorPos(screenWidth / 2, screenHeight / 2);

	moveCursor = true;
}

// �X�V
void Mouse::Update() {

	// �X�C�b�`���
	mouseButton newButtonState = 0;

	for (int i = 0; i < ARRAYSIZE(keyMap); ++i) {
		if (::GetAsyncKeyState(keyMap[i]) & 0x8000) {
			newButtonState |= (1 << i);
		}
	}

	// �z�C�[��
	wheel[1] = wheel[0];
	wheel[0] = 0;

	// �{�^�����X�V
	buttonState[1] = buttonState[0];	// �X�C�b�`����
	buttonState[0] = newButtonState;

	buttonDown = ~buttonState[1] & newButtonState;	// �������u��
	buttonUp = ~newButtonState & buttonState[1];	// �������u��

	// �J�[�\���ʒu�̎擾
	POINT cursor;
	POINT cursorC;
	::GetCursorPos(&cursor);
	::GetCursorPos(&cursorC);
	::ScreenToClient(hWnd, &cursorC);

	// ��ʂ̃T�C�Y���擾����B
	RECT rc;
	::GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = screenWidth;
	UINT viewportH = screenHeight;

	// FPS�ȊO�̃}�E�Xpos
	if (moveCursor) {
		// ��ʕ␳
		positionX[1] = positionX[0];
		positionY[1] = positionY[0];
		positionX[0] = (LONG)(cursorC.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
		positionY[0] = (LONG)(cursorC.y / static_cast<float>(viewportH) * static_cast<float>(screenH));
	}
	// FPS���[�h
	else {
		// �}�E�X�ړ�
		positionX[1] = viewportW / 2;
		positionY[1] = viewportH / 2;
		positionX[0] = (LONG)(cursor.x);
		positionY[0] = (LONG)(cursor.y);

		movePosX = positionX[0] - positionX[1];
		movePosY = positionY[0] - positionY[1];


		//�}�E�X�ʒu�𒆉��ɌŒ�
		::SetCursorPos(viewportW / 2, viewportH / 2);
	}
}