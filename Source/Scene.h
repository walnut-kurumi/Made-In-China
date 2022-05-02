#pragma once

#include <d3d11.h>
#include <memory>
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Fade.h"

// �V�[��
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(float elapsedTime) = 0;

	// �����������Ă��邩
	bool Ready() const { return ready; }

	// ���������ݒ�
	void SetReady() { ready = true; }

	// ���[�h���Z�b�g
	void AddLoadPercent(float l) { LoadPercent += l; }
	void SetLoadPercent(float l) { LoadPercent = l; }
	float GetLoadPercent() { return LoadPercent; }

private:
	bool ready = false;	

public:
	float LoadPercent = 0.0f;
};
