#pragma once
#include "Misc.h"
#include <d3d11.h>
HRESULT loadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc);
HRESULT makeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView, DWORD value/*0xAABBGGRR*/, UINT dimension);
void releaseAllTextures();

ID3D11ShaderResourceView* queryTexture(ID3D11Device* device, const wchar_t* filename, D3D11_TEXTURE2D_DESC* texture2d_desc);
ID3D11ShaderResourceView* queryDummyTexture(ID3D11Device* device, DWORD value/*0xAABBGGRR*/, UINT dimension);