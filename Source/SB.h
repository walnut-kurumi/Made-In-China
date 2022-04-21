#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Vec.h"
#include <memory>
#include <vector>
#include "DebugRenderer.h"

// �O���錾
class SBManager;

// �e��
class SB
{
public:
	SB(SBManager* manager);
	virtual ~SB() {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;
	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
	// �ʒu�擾
	const Vec3& GetPosition() const { return position; }
	// �����擾
	void SetDirection(Vec3 d) { direction = d; }
	// �����擾
	const Vec3& GetDirection() const { return direction; }
	// �X�P�[���擾
	const Vec3& GetScale() const { return scale; }
	// ���a�擾
	float GetRadius() const { return radius; }
	// �X�s�[�h�擾
	float GetSpeed() const { return bulletSpeed; }
	// �j��
	void Destroy();

protected:
	// �s��X�V����
	void UpdateTransform();

protected:

	// �f�o�b�O
	std::unique_ptr<DebugRenderer> debugRenderer;

	Vec3	position = { 0,0,0 };
	Vec3	direction = { 0,0,1 };
	Vec3	scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	SBManager* manager = nullptr;

	float radius = 1.5f;
	float speed = 70.0f;
	float bulletSpeed = 0.0f;
};
