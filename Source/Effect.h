#pragma once

#include<DirectXMath.h>
#include<Effekseer.h>

//エフェクト
class Effect
{
public:
	Effect(const char* filename);
	~Effect();

	//再生
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);

	//停止
	void Stop(Effekseer::Handle handle);

	//座標設定
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	//スケール設定
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

private:
<<<<<<< HEAD
	Effekseer::EffectRef effekseerEffect = nullptr;
=======
	Effekseer::Effect* effekseerEffect = nullptr;
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12

};