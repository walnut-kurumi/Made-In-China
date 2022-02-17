#pragma once

#include "Stage.h"
#include "DebugRenderer.h"

class StageMain : public Stage
{
public:
	StageMain(ID3D11Device* device);
	~StageMain();

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	std::unique_ptr<DebugRenderer> debugRenderer;		
};
