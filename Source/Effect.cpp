#include"Graphics/Graphics.h"
#include"Effect.h"
#include"EffectManager.h"

//コンストラクタ
Effect::Effect(const char* filename)
{
	// エフェクトを読み込みする前にロックする
	// ※マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
	// フリーズする可能性があるので排他制御する
	std::lock_guard<std::mutex> lock(Graphics::Ins().GetMutex());

	//Effekseerのリソースを読み込む
	//EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	//Effekseer::Maagerを取得
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	//Effekseerエフェクトを読み込み
	effekseerEffect = Effekseer::Effect::Create(effekseerManager, (const char16_t*)utf16Filename);
}

//デストラクタ
Effect::~Effect()
{
	//破棄処理
	if (effekseerEffect != nullptr)
	{
		effekseerEffect.Reset();
	}
}

//再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
	
	effekseerManager->SetScale(handle, scale, scale, scale);
	return handle;
}

//再生
Effekseer::Handle Effect::PlayDirection(const DirectX::XMFLOAT3& position, float scale, float radian)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
	
	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
	
	effekseerManager->SetRotation(handle, 0.0f,radian,0.0f);
	effekseerManager->SetScale(handle, scale, scale, scale);
	return handle;
}

//停止
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->StopEffect(handle);
}

//座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

//スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}


// スピード設定
void Effect::SetPlaySpeed(Effekseer::Handle handle, float speed)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetSpeed(handle, speed);
}

// 角度設定
void Effect::SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& radian)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetRotation(handle, radian.x, radian.y, radian.z);
}
