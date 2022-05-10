#pragma once

#include "Stage.h"
#include "Graphics/Sprite.h"
#include "DebugRenderer.h"
#include "../Source/Graphics/ConstantBuffer.h"
#include "Graphics/Vec.h"
#include"Player.h"

class StageCollision2 : public Stage
{
public:
	StageCollision2(ID3D11Device* device);
	~StageCollision2();

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	//void PlayerData(Player* p) { player = p; }

private:

	std::unique_ptr<DebugRenderer> debugRenderer;
	//Player* player = nullptr;
};
