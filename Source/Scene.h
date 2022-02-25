#pragma once

#include <d3d11.h>
#include <memory>

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
	void AddLoadPerf(float l) { LoadPerf += l; }
	void SetLoadPerf(float l) { LoadPerf = l; }
	float GetLoadPerf() { return LoadPerf; }

private:
	bool ready = false;	

public:
	float LoadPerf = 0.0f;
};
