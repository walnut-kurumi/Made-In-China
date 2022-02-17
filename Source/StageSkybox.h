#pragma once

#include "Stage.h"

class StageSkybox : public Stage
{
public:
	StageSkybox(ID3D11Device* device);
	~StageSkybox();

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;
	
};
