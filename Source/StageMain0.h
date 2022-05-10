#pragma once

#include "Stage.h"
#include "Graphics/Sprite.h"
#include "DebugRenderer.h"
#include "../Source/Graphics/ConstantBuffer.h"
#include "Graphics/Vec.h"
#include"Player.h"

class StageMain0 : public Stage
{
public:
	StageMain0(ID3D11Device* device);
	~StageMain0();

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void PlayerData(Player* p) { player = p; }

private:
	ConstantBuffer<Scroll> Scroll;
	Vec2 bgpos{};

	std::unique_ptr<DebugRenderer> debugRenderer;
	Sprite* back{};
	Player* player = nullptr;
}; 
