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

	//��~
	void Stop(Effekseer::Handle handle);

	//���W�ݒ�
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	//�X�P�[���ݒ�
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

private:
<<<<<<< HEAD
	Effekseer::EffectRef effekseerEffect = nullptr;
=======
	Effekseer::Effect* effekseerEffect = nullptr;
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12

};