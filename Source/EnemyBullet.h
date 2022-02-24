#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Vec.h"

// �O���錾
class EnemyBulletManager;

// �e��
class EnemyBullet
{
public:
	EnemyBullet(EnemyBulletManager* manager);
	virtual ~EnemyBullet() {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;
	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;	
	// �ʒu�擾
	const Vec3& GetPosition() const { return position; }
	// �����擾
	void SetDirection(Vec3 d)  { direction = d ; }
	// �����擾
	const Vec3& GetDirection() const { return direction; }
	// �X�P�[���擾
	const Vec3& GetScale() const { return scale; }
	// ���a�擾
	float GetRadius() const { return radius; }
	// �p���B�t���O�ݒ�
	void SetReflectionFlag(bool r) { reflectionFlag = r; }
	// �p���B�t���O�擾
	bool GetReflectionFlag() const { return reflectionFlag; }

	// �j��
	void Destroy();

protected:
	// �s��X�V����
	void UpdateTransform();

protected:
	Vec3	position = { 0,0,0 };
	Vec3	direction = { 0,0,1 };
	Vec3	scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	EnemyBulletManager* manager = nullptr;

	float radius = 0.5f;

	// �p���B�����e���ǂ��� -> �e���Ԃ��Ă���G�ɓ�����悤�ɂȂ�
	bool reflectionFlag = false;
};
