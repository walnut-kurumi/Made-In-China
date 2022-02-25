#pragma once

#include <d3d11.h>
#include <memory>

// ƒV[ƒ“
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(float elapsedTime) = 0;

	// €”õŠ®—¹‚µ‚Ä‚¢‚é‚©
	bool Ready() const { return ready; }

	// €”õŠ®—¹İ’è
	void SetReady() { ready = true; }

private:
	bool ready = false;	
protected:
	float LoadPerf = 0.0f;
};
