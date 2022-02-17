#include "Misc.h"
#include <WICTextureLoader.h>
#include <memory>
#include "Shader.h"
using namespace std;

HRESULT VertexShader::initialize(ID3D11Device* device, const char* csoName, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements) {
	HRESULT hr = S_OK;
	FILE* fp{};

	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> csoData = make_unique<unsigned char[]>(csoSz);
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, shader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	hr = device->CreateInputLayout(inputElementDesc, numElements, csoData.get(), csoSz, inputLayout.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	return hr;
}

HRESULT PixelShader::initialize(ID3D11Device* device, const char* csoName) {
	FILE* fp{};
	HRESULT hr = S_OK;

	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> csoData = make_unique<unsigned char[]>(csoSz);
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	hr = device->CreatePixelShader(csoData.get(), csoSz, nullptr, shader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	return hr;
}

HRESULT GeometryShader::initialize(ID3D11Device* device, const char* csoName) {
	FILE* fp{};
	HRESULT hr = S_OK;

	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> csoData = make_unique<unsigned char[]>(csoSz);
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	hr = device->CreateGeometryShader(csoData.get(), csoSz, nullptr, shader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	return hr;
}

void Shader::InitCSO(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements, const char* vertex, const char* pixel, const char* geometry) {

	vertexshader.initialize(device, vertex, inputElementDesc, numElements);
	pixelshader.initialize(device, pixel);
	if (geometry)geometryshader.initialize(device, geometry);
}

void Shader::InitCSO(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements, const char* filename , bool pixel) {

	if (pixel == false) {
		vertexshader.initialize(device, filename, inputElementDesc, numElements);
	}
	else {
		pixelshader.initialize(device, filename);
	}
}