#pragma once

#include "GamePad.h"
#include "Mouse.h"
#include "Keyboard.h"

// �C���v�b�g
class Input
{
public:
	Input(HWND hWnd);
	~Input() {}

public:
	// �C���X�^���X�擾
	static Input& Instance() { return *ins; }

	// �X�V����
	void update();

	// �Q�[���p�b�h�擾
	GamePad& GetGamePad() { return gamePad; }

	// �}�E�X�擾
	Mouse& GetMouse() { return mouse; }

	// �L�[�{�[�h�擾
	Key& GetKey() { return key; }

private:
	static Input*		ins;
	GamePad				gamePad;
	Mouse				mouse;
	Key                 key;	
};
