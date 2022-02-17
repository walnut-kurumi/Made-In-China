#pragma once

#include <Windows.h>

using mouseButton = unsigned int;

// �}�E�X
class Mouse
{
public:
	static const mouseButton BTN_LEFT = (1 << 0);
	static const mouseButton BTN_MIDDLE = (1 << 1);
	static const mouseButton BTN_RIGHT = (1 << 2);

public:
	Mouse(HWND hWnd);
	~Mouse() {}

	// �X�V
	void Update();

	// �{�^�����͏�Ԃ̎擾
	mouseButton GetButton() const { return buttonState[0]; }

	// �{�^��������Ԃ̎擾
	mouseButton GetButtonDown() const { return buttonDown; }

	// �{�^�������Ԃ̎擾
	mouseButton GetButtonUp() const { return buttonUp; }

	// �z�C�[���l�̐ݒ�
	void SetWheel(int wheel) { this->wheel[0] += wheel; }

	// �z�C�[���l�̎擾
	int GetWheel() const { return wheel[1]; }

	// �}�E�X�J�[�\��X���W�擾
	int GetPositionX() const { return positionX[0]; }

	// �}�E�X�J�[�\��Y���W�擾
	int GetPositionY() const { return positionY[0]; }

	// �O��̃}�E�X�J�[�\��X���W�擾
	int GetOldPositionX() const { return positionX[1]; }

	// �O��̃}�E�X�J�[�\��Y���W�擾
	int GetOldPositionY() const { return positionY[1]; }

	// �}�E�X�J�[�\��X�̈ړ��ʎ擾
	int GetMovePositionX() const { return movePosX; }

	// �}�E�X�J�[�\��Y�̈ړ��ʎ擾
	int GetMovePositionY() const { return movePosY; }

	// �X�N���[�����ݒ�
	void SetScreenWidth(int width) { screenWidth = width; }

	// �X�N���[�������ݒ�
	void SetScreenHeight(int height) { screenHeight = height; }

	// �X�N���[�����擾
	int GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	int GetScreenHeight() const { return screenHeight; }

	void SetMoveCursor(bool s) { moveCursor = s; }

private:
	mouseButton		buttonState[2]{};
	mouseButton		buttonDown = 0;
	mouseButton		buttonUp = 0;
	int				positionX[2];
	int				positionY[2];
	int				movePosX;
	int				movePosY;
	int				wheel[2];
	int				screenWidth = 0;
	int				screenHeight = 0;
	HWND			hWnd{};

	bool moveCursor = false;
};
