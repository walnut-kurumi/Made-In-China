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
	Effekseer::Handle PlayDirection(const DirectX::XMFLOAT3& position, float scale = 1.0f, float radian = 0.0f);

	//停止
	void Stop(Effekseer::Handle handle);

	//座標設定
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	//スケール設定
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);
	
	// スピード設定
	void SetPlaySpeed(Effekseer::Handle handle, float speed);

	// 角度セット
	void SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& radian);

private:
	Effekseer::EffectRef effekseerEffect = nullptr;

};