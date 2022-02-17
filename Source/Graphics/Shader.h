#pragma once
#include <D3D11.h>
#include <wrl.h>

class VertexShader
{
public:
	HRESULT initialize(ID3D11Device* device, const char* csoName, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
	ID3D11VertexShader* getShader() { return this->shader.Get(); }
	ID3D11InputLayout* getInputLayout() { return this->inputLayout.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader
{
public:
	HRESULT initialize(ID3D11Device* device, const char* csoName);
	ID3D11PixelShader* getShader() { return this->shader.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};

class GeometryShader
{
public:
	HRESULT initialize(ID3D11Device* device, const char* csoName);
	ID3D11GeometryShader* getShader() { return this->shader.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> shader;
};


class Shader
{
public:
	VertexShader vertexshader;
	PixelShader pixelshader;
	GeometryShader geometryshader;

public:
	Shader() {}
	virtual ~Shader() {}

	void InitCSO(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements,
		const char* vertex, const char* pixel, const char* geometry = nullptr);
	void InitCSO(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements,
		const char* vertex , bool pixel);

public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		GetVertexShader() { return vertexshader.getShader(); }	
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		GetPixelShader() { return pixelshader.getShader(); }
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GetGeometryShader() { return geometryshader.getShader(); }
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		GetInputLayout() { return vertexshader.getInputLayout(); }
};

