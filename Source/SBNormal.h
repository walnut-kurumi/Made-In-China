#pragma once

#include "Model.h"
#include "SB.h"
#include "SBManager.h"

// ���i�e��
class SBNormal : public SB
{
public:
	SBNormal(ID3D11Device* device, SBManager* manager);
	~SBNormal() override;

	// �X�V����
	void Update(float elapsedTime) override;
	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader)override;
	// ����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float lifeTimer = 2.0f;
};
