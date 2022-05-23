#include "CameraManager.h"
#include "Camera.h"
#include "../Input/Input.h"
#include "../Graphics/Graphics.h"
#include "../StageManager.h"
#include "../Mathf.h"

void CameraManager::Init() {
	Graphics& gfx = Graphics::Ins();
	mainC.SetLookAt(
		Vec3(0.0f, 0.0f, -50.0f),
		Vec3(0.0f, 0.0f, 0.0f),
		Vec3(0.0f, 1.0f, 0.0f)
	);
	float aspectRatio = gfx.GetScreenWidth() / gfx.GetScreenHeight();
	mainC.SetPerspectiveFov(
		DirectX::XMConvertToRadians(30),
		aspectRatio,
		0.1f,
		2000.0f
	);

	// �V�[���萔
	HRESULT hr = this->cbScene.initialize(gfx.GetDevice(), gfx.GetDeviceContext());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	srand((unsigned)time(NULL));
	delay = 0.2f;
}

void CameraManager::Update(float elapsedTime){

	// �^�C�}�[�X�V
	UpdateTimer();

	GamePad& gamePad = Input::Instance().GetGamePad();
	Key& key = Input::Instance().GetKey();
	Mouse& mouse = Input::Instance().GetMouse();

	////�J������]
	//{
	//	float ax = gamePad.GetAxisRX();
	//	float ay = gamePad.GetAxisRY();
	//
	//	//FPS���[�h
	//	if (Input::Instance().GetKey().ALT()) {
	//		ax = mouse.GetMovePositionX();
	//		ay = -mouse.GetMovePositionY();
	//	}
	//
	//
	//	//�J�����̉�]���x
	//	float speed = rollSpeed * elapsedTime;
	//
	//	//�X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
	//	angle.x += ay * speed;
	//	angle.y += ax * speed;
	//
	//	//X���ɃJ������]�𐧌�
	//	if (angle.x > maxAngle) angle.x = maxAngle;
	//	if (angle.x < minAngle) angle.x = minAngle;
	//
	//	if (angle.y < -DirectX::XM_PI) {
	//		angle.y += DirectX::XM_2PI;
	//	}
	//	if (angle.y > DirectX::XM_PI) {
	//		angle.y -= DirectX::XM_2PI;
	//	}
	//}

	// �J�����̈ʒu�̓������肷�鏈��
	{
		//�J������]�l����]�s��ɕϊ�
		DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		//��]�s�񂩂�O�����x�N�g�������o��
		DirectX::XMVECTOR Front = transform.r[2];
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, Front);


		//�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
		// �f�B���C������
		CameraDelay();
		position.x = front.x * range + target.x;
		position.y = front.y * range + target.y;
		position.z = front.z * range + target.z;


		// ��x�N�g��
		Vec3 UP = { 0,1,0 };

		// �I�u�W�F�N�g�ɓ���������J�������߂Â���
		//CameraRay();

		// �J�����V�F�C�N�p����
		camShake.x = static_cast<float>(rand() % 2 )- 1.0f;
		camShake.y = static_cast<float>(rand() % 2 )- 1.0f;
		camShake.z = static_cast<float>(rand() % 2 )- 1.0f;

		// �J�����V�F�C�N
		if (shake)
		{			
			if (timer % 4 == 0)target += camShake;			

			// �J�����V�F�C�N�^�C�}�[�X�V
			UpdateShakeTimer(elapsedTime);
     	}
	
		//range += static_cast<float>(mouse.GetWheel()) * 0.05f;
		if (range <= 0)range = 1.0f;

		//�J�����̎��_�ƒ����_��ݒ�
		mainC.SetLookAt(position, target, UP);

	}
}

void CameraManager::SetBuffer(ID3D11DeviceContext* dc) {

	//�J�����ݒ�	
	DirectX::XMFLOAT4 cameraPosition = { position.x,position.y,position.z,1.0f };

	DirectX::XMStoreFloat4x4(
		&viewProjection,
		DirectX::XMLoadFloat4x4(&mainC.GetView()) * DirectX::XMLoadFloat4x4(&mainC.GetProjection())
	);
	cbScene.data.viewProjection = viewProjection;
	DirectX::XMStoreFloat4x4(&cbScene.data.inverseViewProjection, XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&viewProjection)));
	cbScene.data.lightDirection = { 0.0f, -1.0f, -1.0f, 0.0f };
	cbScene.data.cameraPosition = cameraPosition;
	cbScene.applyChanges();
	dc->VSSetConstantBuffers(1, 1, cbScene.GetAddressOf());
	dc->PSSetConstantBuffers(1, 1, cbScene.GetAddressOf());
	dc->GSSetConstantBuffers(1, 1, cbScene.GetAddressOf());
}

bool CameraManager::CameraRay()
{
	HitResult hit;
	if (StageManager::Instance().RayCast(target, position, hit)) {
		position.x = hit.position.x;
		position.y = hit.position.y;
		position.z = hit.position.z;
		return true;
	}
	return false;
}

void CameraManager::CameraDelay() {

	target.x = Mathf::Lerp(target.x, goal.x, delay);
	target.y = Mathf::Lerp(target.y, goal.y, delay);
	target.z = Mathf::Lerp(target.z, goal.z, delay);

}

// �J�����V�F�C�N�^�C�}�[�X�V
void CameraManager::UpdateShakeTimer(float elapsedTime)
{
	shakeTimer -= elapsedTime;

	if (shakeTimer <= 0.0f) {
		shake = false;
	}
}

// �^�C�}�[�X�V
void CameraManager::UpdateTimer()
{
	timer++;
	if (timer >= 1000)timer = 0;
}
