#pragma once

#include <d3d11.h>
#include <memory>
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Fade.h"

// シーン
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(float elapsedTime) = 0;

	// 準備完了しているか
	bool Ready() const { return ready; }

	// 準備完了設定
	void SetReady() { ready = true; }

	// ロード％セット
	void AddLoadPercent(float l) { LoadPercent += l; }
	void SetLoadPercent(float l) { LoadPercent = l; }
	float GetLoadPercent() { return LoadPercent; }

private:
	bool ready = false;	

public:
	float LoadPercent = 0.0f;
};
