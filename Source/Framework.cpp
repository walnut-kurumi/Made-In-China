#include "Framework.h"
#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "Graphics/Texture.h"
#include "Graphics/Blender.h"
#include"EffectManager.h"


Framework::Framework(HWND hwnd)
	: hwnd(hwnd)
	, input(hwnd)
	, gfx(hwnd){

	//ShowCursor(FALSE);
}

bool Framework::initialize() {
	HRESULT hr = S_OK;

	//カメラ設定
	CameraManager::Create();
	CameraManager::Instance().Init();

	ID3D11Device* device = gfx.GetDevice();
	ID3D11DeviceContext* dc = gfx.GetDeviceContext();

	//エフェクトマネージャー初期化
	EffectManager::Instance().Initialize();

	// シーン初期化
	SceneManager::Instance().Init();
	SceneManager::Instance().ChangeScene(new SceneTitle);


	return true;
}

void Framework::update(float elapsedTime){
	input.update();
	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);

#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	auto& ao = ImGui::GetIO();
	float xScale = static_cast<float>(Graphics::Ins().GetScreenWidth()) / ao.DisplaySize.x;
	float yScale = static_cast<float>(Graphics::Ins().GetScreenHeight()) / ao.DisplaySize.y;
	ao.MousePos = { ao.MousePos.x * xScale, ao.MousePos.y * yScale };

	ImGui::NewFrame();
#endif
}

void Framework::render(float elapsedTime){
	HRESULT hr = S_OK;


	//std::lock_guard<std::mutex> lock(SceneManager::Instance().GetMutex());
	std::lock_guard<std::mutex> lock(Graphics::Ins().GetMutex());
	ID3D11DeviceContext* dc = gfx.GetDeviceContext();
	ID3D11RenderTargetView* rtv = gfx.GetRenderTargetView();	
	ID3D11DepthStencilView* dsv = gfx.GetDepthStencilView();


	FLOAT color[] = { 0.6f,0.6f,0.6f,1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	// 通常レンダリング
	dc->OMSetRenderTargets(1, &rtv, dsv);

	
	CameraManager& cameraMgr = CameraManager::Instance();
	cameraMgr.SetBuffer(dc);

	// シーン描画
	SceneManager::Instance().Render(elapsedTime);
	

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT syncInterval = 1;
	//裏の画面を持ってくる
	gfx.GetSwapChain()->Present(syncInterval, 0);
}


bool Framework::uninitialize(){
	//エフェクトマネージャー終了化
	EffectManager::Instance().Finalize();
	releaseAllTextures();
	return true;
}

void Framework::calculateFrameStats() {
	if (++frames, (tictoc.timeStamp() - elapsedTime) >= 1.0f) {
		float fps = static_cast<float>(frames);
		std::wostringstream outs;
		outs.precision(6);
		outs <<L"katana(kari)" << fps;
		SetWindowTextW(hwnd, outs.str().c_str());

		frames = 0;
		elapsedTime += 1.0f;
	}
}

Framework::~Framework(){
}

int Framework::run() {
	MSG msg{};

	if (!initialize()) {
		return 0;
	}

#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(gfx.GetDevice(), gfx.GetDeviceContext());
	ImGui::StyleColorsDark();
#endif


	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			tictoc.tick();
			calculateFrameStats();
			update(tictoc.time_interval());
			render(tictoc.time_interval());
		}
	}

#ifdef USE_IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

#if 0
	BOOL fullscreen = 0;
	swapChain->GetFullscreenState(&fullscreen, 0);
	if (fullscreen) {
		swapChain->SetFullscreenState(FALSE, 0);
	}
#endif


	return uninitialize() ? static_cast<int>(msg.wParam) : 0;
}

LRESULT Framework::handleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;
#endif

	int  zelta = 0;

	switch (msg) {
	case WM_KEYDOWN: {
		//ESCで終了
		if (wparam == VK_ESCAPE) {
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}

		// キーリピート
		if ((lparam & 0x40000000) != 0) {

		}// 一回目の入力
		else {
			input.GetKey().Set(wparam);
		}
	}
		break;
	case WM_KEYUP: {
		input.GetKey().Release(wparam);
		input.GetKey().ReleaseAlt(wparam);
	}
		break;
	case WM_SYSKEYDOWN: {
		input.GetKey().SetAlt(wparam);
	}
		break;
	case WM_SYSKEYUP:
		input.GetKey().ReleaseAlt(wparam);
		break;
	case WM_MOUSEWHEEL:
		zelta = GET_WHEEL_DELTA_WPARAM(wparam);

		input.GetMouse().SetWheel(-zelta);
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);
		
		EndPaint(hwnd, &ps);
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_ENTERSIZEMOVE:
		tictoc.stop();
		break;
	case WM_EXITSIZEMOVE:
		tictoc.start();
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}