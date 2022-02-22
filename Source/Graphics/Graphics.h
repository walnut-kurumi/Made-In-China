#pragma once

#include <d3d11.h>
#include <WICTextureLoader.h>
#include <wrl.h>
#include <vector>
#include "Blender.h"
#include <vector>
#include <cstdlib>
#include <memory>
#include <iostream>
#include<mutex>

class Graphics
{
public:
	Graphics(HWND hwnd);
	//void render();

	// �C���X�^���X�擾
	static Graphics& Ins() { return *instance; }


private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();

private:
	static Graphics* instance;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	

	D3D11_TEXTURE2D_DESC rampTexture2dDesc;
	D3D11_TEXTURE2D_DESC skyboxTexture2dDesc;		

	float windowWidth;
	float windowHeight;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		solidRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;	
	std::unique_ptr<Blender> blender;

	std::mutex  mutex;

public:

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() const { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() const { return deviceContext.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// �X�N���[�����擾
	float GetScreenWidth() const { return windowWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return windowHeight; }

	ID3D11RasterizerState* GetSolidRasterizerState() { return solidRasterizerState.Get(); }
	ID3D11DepthStencilState* GetDepthStencilState() { return depthStencilState.Get(); }
	Blender* GetBlender() { return blender.get(); }

	//�~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex; }
};
