#pragma once

#include "Model.h"
#include "SB.h"
#include "SBManager.h"

// ’¼i’eŠÛ
class SBNormal : public SB
{
public:
	SBNormal(ID3D11Device* device, SBManager* manager);
	~SBNormal() override;

	// XVˆ—
	void Update(float elapsedTime) override;
	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader)override;
	// ”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float lifeTimer = 2.0f;
};
