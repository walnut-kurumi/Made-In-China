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
	Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
	LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");
	
	// スレッド開始
	std::thread thread(LoadingThread, this);
	// スレッドの管理を放棄
	thread.detach();

	Input::Instance().GetMouse().SetMoveCursor(true);
}

void SceneLoading::Finalize()
{
	delete loadSprite;
	delete Bar;
	delete LoadBar;
}

void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;
	// w = 610 が最大
	w = 61.0f * nextScene->LoadPercent;
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

		Bar->render(dc, 600, 650, 620, 25, 1.0f,1.0f,1.0f,1.0f,0);			
		LoadBar->render(dc, 605, 652, w, 21, 1.0f,1.0f,1.0f,1.0f,0);
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
