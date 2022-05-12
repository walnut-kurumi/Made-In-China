#include "SBNormal.h"
#include "Camera/CameraManager.h"

SBNormal::SBNormal(ID3D11Device* device, SBManager* manager) 
	: SB(manager) {
	model = new Model(device, "Data/Models/Bullet/bullet.fbx", true, 0);
	// �\���T�C�Y�𒲐�
	scale.x = scale.y = scale.z = 1.0f;

	debugRenderer = std::make_unique<DebugRenderer>(device);
}

SBNormal::~SBNormal() {
	delete model;
}
// �X�V����
void SBNormal::Update(float elapsedTime) {
	Vec3 m = position;
	// ��������
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f) {
		// �������폜
		Destroy();
	}
	// �ړ�
	bulletSpeed = this->speed * elapsedTime;
	position.x += direction.x * bulletSpeed;
	position.y += direction.y * bulletSpeed;
	position.z += direction.z * bulletSpeed;

	// �ړ�����
	moveLen += position - m;
	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();
	// ���f���s��X�V
	model->UpdateTransform(transform);
}
// �`�揈��
void SBNormal::Render(ID3D11DeviceContext* dc, Shader* shader) {
	model->Begin(dc, *shader);
	model->Render(dc);
#ifdef _DEBUG
	//// �K�v�Ȃ�����ǉ�
	debugRenderer.get()->DrawSphere(position, radius, Vec4(1, 0.5f, 0, 1));

	//debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
#endif
}

// ���ˏ���
void SBNormal::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position) {
	this->direction = direction;
	this->position = position;
}
