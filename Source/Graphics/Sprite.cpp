#include "Sprite.h"
#include "Texture.h"
#include "Misc.h"
#include <sstream>
#include <WICTextureLoader.h>

#include "Shaders.h"

Sprite::Sprite(ID3D11Device* device, const wchar_t* filename) {
	HRESULT hr = S_OK;

	//���_���̃Z�b�g
	Vertex vertices[] = {
		{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
		{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
		{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
		{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
	};

	//���_�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(vertices);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
	}
	D3D11_SUBRESOURCE_DATA subresourceData{};
	{
		subresourceData.pSysMem = vertices;
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
	}
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, this->vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	//�摜�t�@�C���̃��[�h�@�V�F�[�_���\�[�X�r���[�I�u�W�F�N�g����
	loadTextureFromFile(device, filename, this->shaderResourceView.GetAddressOf(), &texture2dDesc);


	// �[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC depthstencildesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
		{
			depthstencildesc.DepthEnable = FALSE;
			depthstencildesc.StencilEnable = TRUE;
			depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;
		}
		hr = device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ���X�^���C�U�[�X�e�[�g
	{
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		{
			hr = device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		}
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	//�T���v���[�X�e�[�g
	{
		//�T���v���[�X�e�[�g�I�u�W�F�N�g����
		CD3D11_SAMPLER_DESC samplerDesc{};
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		}
		hr = device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	//�u�����h
	{
		//�u�����f�B���O�X�e�[�g�I�u�W�F�N�g����
		blender = std::make_unique<Blender>(device);
	}
}

Sprite::Sprite(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView)
{
	HRESULT hr{ S_OK };

	Vertex vertices[]
	{
		{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
		{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
		{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
		{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
	};

	//���_�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(vertices);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
	}
	D3D11_SUBRESOURCE_DATA subresourceData{};
	{
		subresourceData.pSysMem = vertices;
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
	}
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, this->vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	// �[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC depthstencildesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
		{
			depthstencildesc.DepthEnable = FALSE;
			depthstencildesc.StencilEnable = TRUE;
			depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
			depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;
		}
		hr = device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ���X�^���C�U�[�X�e�[�g
	{
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		{
			hr = device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		}
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	//�T���v���[�X�e�[�g
	{
		//�T���v���[�X�e�[�g�I�u�W�F�N�g����
		CD3D11_SAMPLER_DESC samplerDesc{};
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		}
		hr = device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	//�u�����h
	{
		//�u�����f�B���O�X�e�[�g�I�u�W�F�N�g����
		blender = std::make_unique<Blender>(device);
	}
	

	if (shaderResourceView)
	{
		isLoadFile = false;
		shaderResourceView.Get()->AddRef();
		this->shaderResourceView = shaderResourceView;
		// 
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		this->shaderResourceView->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		texture2d->GetDesc(&texture2dDesc);
	}
}

void Sprite::render(ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh) {
	render(deviceContext, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void Sprite::render(ID3D11DeviceContext* deviceContext, 
	float dx, float dy, float dw, float dh, 
	float r, float g, float b, float a,
	float angle) {
	render(deviceContext, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void Sprite::render(ID3D11DeviceContext* deviceContext, 
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle,
	float sx, float sy, float sw, float sh){
	HRESULT hr = S_OK;
	//�r���[�|�[�g�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &viewport);

	//�Z�a�̊e���_�̈ʒu���v�Z
	float ltx = dx, lty = dy;			 //left  - top
	float rtx = dx + dw, rty = dy;		 //right - top
	float lbx = dx, lby = dy + dh;		 //left  - bottom
	float rbx = dx + dw, rby = dy + dh;	 //right - bottom

	//�����_���ɂ���]�v�Z
	//auto rotate = [](float& x, float& y, float cx, float cy, float angle) {
	//	x -= cx;
	//	y -= cy;

	//	float cos = cosf(DirectX::XMConvertToRadians(angle));
	//	float sin = sinf(DirectX::XMConvertToRadians(angle));
	//	float tx = x, ty = y;
	//	x = cos * tx + -sin * ty;
	//	y = sin * tx + cos * ty;

	//	x += cx;
	//	y += cy;
	//};
	//��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	float cos = cosf(DirectX::XMConvertToRadians(angle));
	float sin = sinf(DirectX::XMConvertToRadians(angle));
	//��]�v�Z�֐�
	//rotate(ltx, lty, cx, cy, angle);
	//rotate(rtx, rty, cx, cy, angle);
	//rotate(lbx, lby, cx, cy, angle);
	//rotate(rbx, rby, cx, cy, angle);
	rotate(ltx, lty, cx, cy, cos, sin);
	rotate(rtx, rty, cx, cy, cos, sin);
	rotate(lbx, lby, cx, cy, cos, sin);
	rotate(rbx, rby, cx, cy, cos, sin);


	//Convert to NDC space
	ltx = 2.0f * ltx / viewport.Width - 1.0f;
	lty = 1.0f - 2.0f * lty / viewport.Height;
	rtx = 2.0f * rtx / viewport.Width - 1.0f;
	rty = 1.0f - 2.0f * rty / viewport.Height;
	lbx = 2.0f * lbx / viewport.Width - 1.0f;
	lby = 1.0f - 2.0f * lby / viewport.Height;
	rbx = 2.0f * rbx / viewport.Width - 1.0f;
	rby = 1.0f - 2.0f * rby / viewport.Height;



	//�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = deviceContext->Map(this->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	Vertex* vertices = reinterpret_cast<Vertex*>(mappedSubresource.pData);
	if (vertices != nullptr) {
		vertices[0].position = { ltx, lty , 0 };
		vertices[1].position = { rtx, rty , 0 };
		vertices[2].position = { lbx, lby , 0 };
		vertices[3].position = { rbx, rby , 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

		vertices[0].texcoord = { sx / texture2dDesc.Width, sy / texture2dDesc.Height };
		vertices[1].texcoord = { (sx + sw) / texture2dDesc.Width, sy / texture2dDesc.Height };
		vertices[2].texcoord = { sx / texture2dDesc.Width, (sy + sh) / texture2dDesc.Height };
		vertices[3].texcoord = { (sx + sw) / texture2dDesc.Width, (sy + sh) / texture2dDesc.Height };
	}
	deviceContext->Unmap(this->vertexBuffer.Get(), 0);

	//���_�o�b�t�@�̃o�C���h
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &stride, &offset);
	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�V�F�[�_���\�[�X�̃o�C���h
	deviceContext->PSSetShaderResources(0, 1, this->shaderResourceView.GetAddressOf());


	//Sprite Shader
	Shader shader = Shaders::Ins()->GetSpriteShader();
	// ���̓��C�A�E�g
	deviceContext->IASetInputLayout(shader.GetInputLayout().Get());
	// �V�F�[�_�[�̐ݒ�
	deviceContext->VSSetShader(shader.GetVertexShader().Get(), nullptr, 0);
	deviceContext->PSSetShader(shader.GetPixelShader().Get(), nullptr, 0);

	//blending state object �ݒ�
	blender->setBlendMode(Blender::BLEND_STATE::BS_ALPHA, deviceContext);

	//�T���v���[�X�e�[�g�I�u�W�F�N�g�o�C���h
	deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	
	//���X�^���C�U�X�e�[�g�ݒ�
	deviceContext->RSSetState(rasterizerState.Get());

	//�[�x�X�e���V���X�e�[�g�I�u�W�F�N�g��ݒ�
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);

	//�v���~�e�B�u�̕`��
	deviceContext->Draw(4, 0);


	ID3D11ShaderResourceView* srvs[] = { nullptr };
	deviceContext->PSSetShaderResources(0, 1, srvs);

}

void Sprite::render(ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, Shader shader)
{
	render(deviceContext, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height),shader);
}

void Sprite::render(ID3D11DeviceContext* deviceContext,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle, Shader shader)
{
	render(deviceContext, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height),shader);
}

void Sprite::render(ID3D11DeviceContext* deviceContext,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle,
	float sx, float sy, float sw, float sh, Shader shader)
{
	HRESULT hr = S_OK;
	//�r���[�|�[�g�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &viewport);

	//�Z�a�̊e���_�̈ʒu���v�Z
	float ltx = dx, lty = dy;			 //left  - top
	float rtx = dx + dw, rty = dy;		 //right - top
	float lbx = dx, lby = dy + dh;		 //left  - bottom
	float rbx = dx + dw, rby = dy + dh;	 //right - bottom

	//�����_���ɂ���]�v�Z
	//auto rotate = [](float& x, float& y, float cx, float cy, float angle) {
	//	x -= cx;
	//	y -= cy;

	//	float cos = cosf(DirectX::XMConvertToRadians(angle));
	//	float sin = sinf(DirectX::XMConvertToRadians(angle));
	//	float tx = x, ty = y;
	//	x = cos * tx + -sin * ty;
	//	y = sin * tx + cos * ty;

	//	x += cx;
	//	y += cy;
	//};
	//��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	float cos = cosf(DirectX::XMConvertToRadians(angle));
	float sin = sinf(DirectX::XMConvertToRadians(angle));
	//��]�v�Z�֐�
	//rotate(ltx, lty, cx, cy, angle);
	//rotate(rtx, rty, cx, cy, angle);
	//rotate(lbx, lby, cx, cy, angle);
	//rotate(rbx, rby, cx, cy, angle);
	rotate(ltx, lty, cx, cy, cos, sin);
	rotate(rtx, rty, cx, cy, cos, sin);
	rotate(lbx, lby, cx, cy, cos, sin);
	rotate(rbx, rby, cx, cy, cos, sin);


	//Convert to NDC space
	ltx = 2.0f * ltx / viewport.Width - 1.0f;
	lty = 1.0f - 2.0f * lty / viewport.Height;
	rtx = 2.0f * rtx / viewport.Width - 1.0f;
	rty = 1.0f - 2.0f * rty / viewport.Height;
	lbx = 2.0f * lbx / viewport.Width - 1.0f;
	lby = 1.0f - 2.0f * lby / viewport.Height;
	rbx = 2.0f * rbx / viewport.Width - 1.0f;
	rby = 1.0f - 2.0f * rby / viewport.Height;



	//�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = deviceContext->Map(this->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	Vertex* vertices = reinterpret_cast<Vertex*>(mappedSubresource.pData);
	if (vertices != nullptr) {
		vertices[0].position = { ltx, lty , 0 };
		vertices[1].position = { rtx, rty , 0 };
		vertices[2].position = { lbx, lby , 0 };
		vertices[3].position = { rbx, rby , 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

		vertices[0].texcoord = { sx / texture2dDesc.Width, sy / texture2dDesc.Height };
		vertices[1].texcoord = { (sx + sw) / texture2dDesc.Width, sy / texture2dDesc.Height };
		vertices[2].texcoord = { sx / texture2dDesc.Width, (sy + sh) / texture2dDesc.Height };
		vertices[3].texcoord = { (sx + sw) / texture2dDesc.Width, (sy + sh) / texture2dDesc.Height };
	}
	deviceContext->Unmap(this->vertexBuffer.Get(), 0);

	//���_�o�b�t�@�̃o�C���h
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &stride, &offset);
	//�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�V�F�[�_���\�[�X�̃o�C���h
	deviceContext->PSSetShaderResources(0, 1, this->shaderResourceView.GetAddressOf());

	
	// ���̓��C�A�E�g
	deviceContext->IASetInputLayout(shader.GetInputLayout().Get());
	// �V�F�[�_�[�̐ݒ�
	deviceContext->VSSetShader(shader.GetVertexShader().Get(), nullptr, 0);
	deviceContext->PSSetShader(shader.GetPixelShader().Get(), nullptr, 0);

	//blending state object �ݒ�
	blender->setBlendMode(Blender::BLEND_STATE::BS_ALPHA, deviceContext);

	//�T���v���[�X�e�[�g�I�u�W�F�N�g�o�C���h
	deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	//���X�^���C�U�X�e�[�g�ݒ�
	deviceContext->RSSetState(rasterizerState.Get());

	//�[�x�X�e���V���X�e�[�g�I�u�W�F�N�g��ݒ�
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);

	//�v���~�e�B�u�̕`��
	deviceContext->Draw(4, 0);


	ID3D11ShaderResourceView* srvs[] = { nullptr };
	deviceContext->PSSetShaderResources(0, 1, srvs);

}

void Sprite::textout(ID3D11DeviceContext* deviceContext, std::string s,
	float x, float y, float w, float h, float r, float g, float b, float a) {

	float sw = static_cast<float>(texture2dDesc.Width / 16);
	float sh = static_cast<float>(texture2dDesc.Height / 16);
	float carriage = 0;
	for (const char c : s) {
		render(deviceContext, x + carriage, y, w, h, r, g, b, a, 0,
			sw * (c & 0x0F), sh * (c >> 4), sw, sh);
		carriage += w;
	}
}

Sprite::~Sprite(){
	if (!isLoadFile)
		shaderResourceView->Release();

}
