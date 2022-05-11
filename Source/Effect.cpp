#include"Graphics/Graphics.h"
#include"Effect.h"
#include"EffectManager.h"

//�R���X�g���N�^
Effect::Effect(const char* filename)
{
	// �G�t�F�N�g��ǂݍ��݂���O�Ƀ��b�N����
	// ���}���`�X���b�h��Effect���쐬�����DeviceContext�𓯎��A�N�Z�X����
	// �t���[�Y����\��������̂Ŕr�����䂷��
	std::lock_guard<std::mutex> lock(Graphics::Ins().GetMutex());

	//Effekseer�̃��\�[�X��ǂݍ���
	//Effekseer��UTF-16�̃t�@�C���p�X�ȊO�͑Ή����Ă��Ȃ����ߕ����R�[�h�ϊ����K�v
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	//Effekseer::Maager���擾
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	//Effekseer�G�t�F�N�g��ǂݍ���
	effekseerEffect = Effekseer::Effect::Create(effekseerManager, (const char16_t*)utf16Filename);
}

//�f�X�g���N�^
Effect::~Effect()
{
	//�j������
	if (effekseerEffect != nullptr)
	{
		effekseerEffect.Reset();
	}
}

//�Đ�
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
	
	effekseerManager->SetScale(handle, scale, scale, scale);
	return handle;
}

//�Đ�
Effekseer::Handle Effect::PlayDirection(const DirectX::XMFLOAT3& position, float scale, float radian)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
	
	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
	
	effekseerManager->SetRotation(handle, 0.0f,radian,0.0f);
	effekseerManager->SetScale(handle, scale, scale, scale);
	return handle;
}

//��~
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->StopEffect(handle);
}

//���W�ݒ�
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

//�X�P�[���ݒ�
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}


// �X�s�[�h�ݒ�
void Effect::SetPlaySpeed(Effekseer::Handle handle, float speed)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetSpeed(handle, speed);
}

// �p�x�ݒ�
void Effect::SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& radian)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetRotation(handle, radian.x, radian.y, radian.z);
}
