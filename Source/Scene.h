#pragma once

#include <d3d11.h>
#include <memory>

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
	void AddLoadPerf(float l) { LoadPerf += l; }
	void SetLoadPerf(float l) { LoadPerf = l; }
	float GetLoadPerf() { return LoadPerf; }

private:
	bool ready = false;	

public:
	float LoadPerf = 0.0f;
};
