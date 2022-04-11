#include "Mouse.h"
#include "Input.h"

static const int keyMap[] = {
	VK_LBUTTON,		// 左ボタン
	VK_MBUTTON,		// 中ボタン
	VK_RBUTTON,		// 右ボタン
};

// コンストラクタ
Mouse::Mouse(HWND hWnd)
	: hWnd(hWnd) {
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;


	::SetCursorPos(screenWidth / 2, screenHeight / 2);

	moveCursor = true;
}

// 更新
void Mouse::Update() {

	// スイッチ情報
	mouseButton newButtonState = 0;

	for (int i = 0; i < ARRAYSIZE(keyMap); ++i) {
		if (::GetAsyncKeyState(keyMap[i]) & 0x8000) {
			newButtonState |= (1 << i);
		}
	}

	// ホイール
	wheel[1] = wheel[0];
	wheel[0] = 0;

	// ボタン情報更新
	buttonState[1] = buttonState[0];	// スイッチ履歴
	buttonState[0] = newButtonState;

	buttonDown = ~buttonState[1] & newButtonState;	// 押した瞬間
	buttonUp = ~newButtonState & buttonState[1];	// 離した瞬間

	// カーソル位置の取得
	POINT cursor;
	POINT cursorC;
	::GetCursorPos(&cursor);
	::GetCursorPos(&cursorC);
	::ScreenToClient(hWnd, &cursorC);

	// 画面のサイズを取得する。
	RECT rc;
	::GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = screenWidth;
	UINT viewportH = screenHeight;

	// FPS以外のマウスpos
	if (moveCursor) {
		// 画面補正
		positionX[1] = positionX[0];
		positionY[1] = positionY[0];
		positionX[0] = (LONG)(cursorC.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
		positionY[0] = (LONG)(cursorC.y / static_cast<float>(viewportH) * static_cast<float>(screenH));
	}
	// FPSモード
	else {
		// マウス移動
		positionX[1] = viewportW / 2;
		positionY[1] = viewportH / 2;
		positionX[0] = (LONG)(cursor.x);
		positionY[0] = (LONG)(cursor.y);

		movePosX = positionX[0] - positionX[1];
		movePosY = positionY[0] - positionY[1];


		//マウス位置を中央に固定
		::SetCursorPos(viewportW / 2, viewportH / 2);
	}
}