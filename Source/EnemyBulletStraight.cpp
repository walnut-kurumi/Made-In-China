#include "EnemyBulletStraight.h"
#include "Camera/CameraManager.h"

EnemyBulletStraight::EnemyBulletStraight(ID3D11Device* device, EnemyBulletManager* manager):EnemyBullet(manager)
{	
	model = std::make_unique<Model>(device,"Data/Models/Bullet/bullet.fbx",true,0);
	// �\���T�C�Y�𒲐�
	scale.x = scale.y = scale.z = 1.0f;	

	ballisticEffect = new Effect("Data/Effect/enemyBullet.efk");

	debugRenderer = std::make_unique<DebugRenderer>(device);
}

EnemyBulletStraight::~EnemyBulletStraight()
{	
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
		ballisticEffect->Stop(handle);
	}

	// �ړ�
	bulletSpeed = this->speed * elapsedTime;
	position.x += direction.x * bulletSpeed;
	position.y += direction.y * bulletSpeed;
	position.z += direction.z * bulletSpeed;

	// �G�t�F�N�g���W
	ballisticEffect->SetPosition(handle, position);

	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();
	// ���f���s��X�V
	model->UpdateTransform(transform);

	CollisionVsStage();
}
// �`�揈��
void EnemyBulletStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	model->Begin(dc, *shader);
	model->Render(dc);


#ifdef _DEBUG
	//// �K�v�Ȃ�����ǉ�
	debugRenderer.get()->DrawSphere(position, radius, Vec4(1, 0.5f, 0, 1));

	debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
#endif
}

// ���ˏ���
void EnemyBulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{	

	this->direction = direction;
	this->position = position;

	float angle = 90;
	if (direction.x > 0)angle = -90;
	else angle = 90;
	float radian = DirectX::XMConvertToRadians(angle);
	handle = ballisticEffect->PlayDirection(position, 1.0f, radian);
	ballisticEffect->SetSpeed(handle, 8.0f);
}
