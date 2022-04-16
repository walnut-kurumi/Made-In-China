#pragma once

#include "Stage.h"
#include "Graphics/Sprite.h"
#include "DebugRenderer.h"
#include "../Source/Graphics/ConstantBuffer.h"
#include"Player.h"

class StageMain : public Stage
{
public:
	StageMain(ID3D11Device* device);
	~StageMain();

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void PlayerData(Player* p) { player = p; }

private:

	ConstantBuffer<Scroll> Scroll;

	std::unique_ptr<DebugRenderer> debugRenderer;		
	Sprite* back{};
	Player* player = nullptr;
};
