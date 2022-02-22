#include"Graphics/Graphics.h"
#include"Effect.h"
#include"EffectManager.h"

//コンストラクタ
Effect::Effect(const char* filename)
{
<<<<<<< HEAD
=======
	// エフェクトを読み込みする前にロックする
	// ※マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
	// フリーズする可能性があるので排他制御する
	std::lock_guard<std::mutex> lock(Graphics::Ins().GetMutex());

>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
	//Effekseerのリソースを読み込む
	//EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

<<<<<<< HEAD
	//Effekseer::Maagerを取得
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
=======
	//Effekseer::Managerを所得
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12

	//Effekseerエフェクトを読み込み
	effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

//デストラクタ
Effect::~Effect()
{
	//破棄処理
	if (effekseerEffect != nullptr)
	{
		effekseerEffect->Release();
		effekseerEffect = nullptr;
	}
}

//再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
<<<<<<< HEAD
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);

	effekseerManager->SetScale(handle, scale, scale, scale);

=======
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
	effekseerManager->SetScale(handle, scale, scale, scale);
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
	return handle;
}

//停止
void Effect::Stop(Effekseer::Handle handle)
{
<<<<<<< HEAD
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
=======
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12

	effekseerManager->StopEffect(handle);
}

//座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
<<<<<<< HEAD
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
=======
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

//スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
<<<<<<< HEAD
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
=======
	Effekseer::Manager* effekseerManager = EffectManager::Instance().GetEffekseerManager();
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}