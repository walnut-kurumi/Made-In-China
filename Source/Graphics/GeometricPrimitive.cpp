#include "GeometricPrimitive.h"
#include "Misc.h"

GeometricPrimitive::GeometricPrimitive(ID3D11Device* device){
	
	createSquare(device);

	HRESULT hr = S_OK;
	
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vertexShader.initialize(device, "shader\\obj\\GeometricPrimitiveVs.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	pixelShader.initialize(device, "shader\\obj\\GeometricPrimitivePs.cso");
	//createVsFromCso(device, "shader\\obj\\GeometricPrimitiveVs.cso", this->vertexShader.GetAddressOf(),
	//	inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	//createPsFromCso(device, "shader\\obj\\GeometricPrimitivePs.cso", this->pixelShader.GetAddressOf());
	
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(Constants);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	}
	hr = device->CreateBuffer(&bufferDesc, nullptr, this->constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}


void GeometricPrimitive::createSquare(ID3D11Device* device) {
	Vertex vertices[24] = {};
	// �T�C�Y�� 1.0 �̐������̃f�[�^���쐬����i�d�S�����_�ɂ���j�B�������̂̃R���g���[���|�C���g���� 8 �A
	// 1 �̃R���g���[���|�C���g�̈ʒu�ɂ͖@���̌������Ⴄ���_�� 3 ����̂Œ��_���̑����� 8x3=24 �A
	// ���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B

	uint32_t indices[36] = {};
	// �������̂� 6 �ʎ����A1 �̖ʂ� 2 �� 3 �p�`�|���S���ō\�������̂� 3 �p�`�|���S���̑����� 6x2=12 �A
	// �������̂�`�悷�邽�߂� 12 ��� 3 �p�`�|���S���`�悪�K�v�A����ĎQ�Ƃ���钸�_���� 12x3=36 ��A
	// 3 �p�`�|���S�����Q�Ƃ��钸�_���̃C���f�b�N�X�i���_�ԍ��j��`�揇�ɔz��iindices�j�Ɋi�[����B
	// ���v��肪�\�ʂɂȂ�悤�Ɋi�[���邱�ƁB


	uint32_t face = 0;

	// top-side
	// 0---------1
	// |         |
	// |   -Y    |
	// |         |
	// 2---------3
	face = 0;
	vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 3].position = { +0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, +1.0f, +0.0f };
	vertices[face * 4 + 1].normal = { +0.0f, +1.0f, +0.0f };
	vertices[face * 4 + 2].normal = { +0.0f, +1.0f, +0.0f };
	vertices[face * 4 + 3].normal = { +0.0f, +1.0f, +0.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// bottom-side
	// 0---------1
	// |         |
	// |   -Y    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = { -0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 1].position = { +0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, -1.0f, +0.0f };
	vertices[face * 4 + 1].normal = { +0.0f, -1.0f, +0.0f };
	vertices[face * 4 + 2].normal = { +0.0f, -1.0f, +0.0f };
	vertices[face * 4 + 3].normal = { +0.0f, -1.0f, +0.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	// front-side
	// 0---------1
	// |         |
	// |   +Z    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 1].position = { +0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, +0.0f, -1.0f };
	vertices[face * 4 + 1].normal = { +0.0f, +0.0f, -1.0f };
	vertices[face * 4 + 2].normal = { +0.0f, +0.0f, -1.0f };
	vertices[face * 4 + 3].normal = { +0.0f, +0.0f, -1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// back-side
	// 0---------1
	// |         |
	// |   +Z    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 2].position = { -0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 0].normal = { +0.0f, +0.0f, +1.0f };
	vertices[face * 4 + 1].normal = { +0.0f, +0.0f, +1.0f };
	vertices[face * 4 + 2].normal = { +0.0f, +0.0f, +1.0f };
	vertices[face * 4 + 3].normal = { +0.0f, +0.0f, +1.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	// right-side
	// 0---------1
	// |         |      
	// |   -X    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = { +0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 2].position = { +0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 0].normal = { +1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 1].normal = { +1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 2].normal = { +1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 3].normal = { +1.0f, +0.0f, +0.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// left-side
	// 0---------1
	// |         |      
	// |   -X    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
	vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
	vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
	vertices[face * 4 + 3].position = { -0.5f, -0.5f, +0.5f };
	vertices[face * 4 + 0].normal = { -1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 1].normal = { -1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 2].normal = { -1.0f, +0.0f, +0.0f };
	vertices[face * 4 + 3].normal = { -1.0f, +0.0f, +0.0f };
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	createComBuffers(device, vertices, 24, indices, 36);
}


void GeometricPrimitive::render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor){
	uint32_t stride = sizeof(Vertex);
	uint32_t offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(vertexShader.getInputLayout());

	deviceContext->VSSetShader(this->vertexShader.getShader(), nullptr, 0);
	deviceContext->PSSetShader(this->pixelShader.getShader(), nullptr, 0);

	Constants data = { world, materialColor };
	deviceContext->UpdateSubresource(this->constantBuffer.Get(), 0, nullptr, &data, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	D3D11_BUFFER_DESC bufferDesc{};
	this->indexBuffer->GetDesc(&bufferDesc);
	deviceContext->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
}

void GeometricPrimitive::createComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount, uint32_t* indices, size_t indexCount){
	HRESULT hr = S_OK;
	
	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subresourceData{};
	{
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices;
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
	}
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, this->vertexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));	
	{
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresourceData.pSysMem = indices;
	}
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, this->indexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}
