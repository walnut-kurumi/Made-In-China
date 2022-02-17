#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include <thread>
#include "Graphics/Graphics.h"


SceneLoading::SceneLoading(Scene* nextScene)
{
	this->nextScene = nextScene;
}

void SceneLoading::Initialize()
{
	ID3D11Device* device = Graphics::Ins().GetDevice();
	// スプライト初期化	
	loadSprite = new Sprite(device, L"./Data/Sprites//loading.png");
	
	// スレッド開始
	std::thread thread(LoadingThread, this);
	// スレッドの管理を放棄
	thread.detach();

	Input::Instance().GetMouse().SetMoveCursor(true);
}

void SceneLoading::Finalize()
{
}

void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	// 次のシーンの準備が完了したらシーンを切り替える
	if (nextScene->Ready())
	{
		SceneManager::Instance().ChangeScene(nextScene);
		nextScene = nullptr;
	}
}

void SceneLoading::Render(float elapsedTime)
{
	ID3D11Device* device = Graphics::Ins().GetDevice();
	ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
	Graphics& gfx = Graphics::Ins();

	// 2D描画
	{
		loadSprite->render(dc, 0, 0, 256, 256,1.0f,1.0f,1.0f,1.0f,angle);
	}
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);
	// 次のシーンの初期化を行う
	scene->nextScene->Initialize();
	// スレッドが終わる前にCOM関連の終了化
	CoUninitialize();
	// 次のシーンの準備完了設定
	scene->nextScene->SetReady();
}
