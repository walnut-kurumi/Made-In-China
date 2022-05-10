#pragma once

#include<DirectXMath.h>
#include<Effekseer.h>

//�G�t�F�N�g
class Effect
{
public:
	Effect(const char* filename);
	~Effect();

	//�Đ�
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);
	Effekseer::Handle PlayDirection(const DirectX::XMFLOAT3& position, float scale = 1.0f, float radian = 0.0f);

	//��~
	void Stop(Effekseer::Handle handle);

	//���W�ݒ�
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	//�X�P�[���ݒ�
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);
	
	// �X�s�[�h�ݒ�
	void SetPlaySpeed(Effekseer::Handle handle, float speed);

	// �p�x�Z�b�g
	void SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& radian);

private:
	Effekseer::EffectRef effekseerEffect = nullptr;

};