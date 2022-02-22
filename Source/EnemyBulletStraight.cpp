#include "EnemyBulletStraight.h"

EnemyBulletStraight::EnemyBulletStraight(ID3D11Device* device, EnemyBulletManager* manager):EnemyBullet(manager)
{	
	model = new Model(device,"Data/Models/Bullet/bullet.fbx",true,0);

	// �\���T�C�Y�𒲐�
	scale.x = scale.y = scale.z = 0.05f;	
}

EnemyBulletStraight::~EnemyBulletStraight()
{
	delete model;
}
// �X�V����
void EnemyBulletStraight::Update(float elapsedTime)
{
	// ��������
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f)
	{
		// �������폜
		Destroy();
	}
	// �ړ�
	float speed = this->speed * elapsedTime;
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;
	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();
	// ���f���s��X�V
	model->UpdateTransform(transform);
}
// �`�揈��
void EnemyBulletStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	model->Begin(dc, *shader);
	model->Render(dc);
}

// ���ˏ���
void EnemyBulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
