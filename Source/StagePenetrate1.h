#pragma once

#include "Stage.h"
#include "Graphics/Sprite.h"
#include "DebugRenderer.h"
#include "../Source/Graphics/ConstantBuffer.h"
#include "Graphics/Vec.h"

class StagePenetrate1 : public Stage
{
public:
	StagePenetrate1(ID3D11Device* device);
	~StagePenetrate1();

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;	

private:
	std::unique_ptr<DebugRenderer> debugRenderer;	
};