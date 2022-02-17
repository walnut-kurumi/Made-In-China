#pragma once

#include <tchar.h>
#include <sstream>
#include <Windows.h>

#include "HighResolutionTimer.h"
#include <d3d11.h>
#include <wrl.h>
#include "Graphics/Misc.h"

#include "Graphics/Graphics.h"
#include "Input/Input.h"


#ifdef USE_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LPCWSTR APPLICATION_NAME = L"X3DGP";

class Framework
{
public:
	CONST HWND hwnd;

	Graphics gfx;
	Input input;

public:

	Framework(HWND hwnd);
	~Framework();

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	int run();

	LRESULT CALLBACK handleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	bool initialize();
	void update(float elapsedTime);
	void render(float elapsedTime);
	bool uninitialize();

private:
	HighResolutionTimer tictoc;
	uint32_t frames = 0;
	float elapsedTime = 0.0f;
	void calculateFrameStats();
	
};

