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
	// �X�v���C�g������	
	backSprite = new Sprite(device, L"./Data/Sprites/scene/black.png");
	loadSprite = new Sprite(device, L"./Data/Sprites//loading.png");
	Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
	LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");
	
	// �X���b�h�J�n
	std::thread thread(LoadingThread, this);
	// �X���b�h�̊Ǘ������
	thread.detach();

	Input::Instance().GetMouse().SetMoveCursor(true);
}

void SceneLoading::Finalize()
{
	delete backSprite;
	delete loadSprite;
	delete Bar;
	delete LoadBar;
}

void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;
	// w = 610 ���ő�
	w = 61.0f * nextScene->LoadPercent;
	// ���̃V�[���̏���������������V�[����؂�ւ���
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
	ID3D11RenderTargetView* rtv = gfx.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = gfx.GetDepthStencilView();


	FLOAT color[] = { 0.6f,0.6f,0.6f,1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// �ʏ탌���_�����O
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2D�`��
	{
		backSprite->render(dc, 0, 0, 1920, 1080,1.0f,1.0f,1.0f,1.0f,0);
	
		loadSprite->render(dc, 0, 0, 256, 256,1.0f,1.0f,1.0f,1.0f,angle);

		Bar->render(dc, 600, 650, 620, 25, 1.0f,1.0f,1.0f,1.0f,0);			
		LoadBar->render(dc, 605, 652, w, 21, 1.0f,1.0f,1.0f,1.0f,0);
	}
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);
	// ���̃V�[���̏��������s��
	scene->nextScene->Initialize();
	// �X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();
	// ���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();
}
