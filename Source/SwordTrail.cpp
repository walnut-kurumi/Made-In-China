#include "SwordTrail.h"
#include "Graphics/Misc.h"
#include "Graphics/Shaders.h"

SwordTrail::SwordTrail(ID3D11Device* device, ID3D11DeviceContext* dc) {
	// �萔�o�b�t�@
	trailCb.initialize(device, dc);

	//�u�����f�B���O�X�e�[�g
	blender = std::make_unique<Blender>(device);

	// �[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ���X�^���C�U�[�X�e�[�g
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}
}

void SwordTrail::SetSwordPos(const Vec3& topPos, const Vec3& bottomPos) {
	TrailBuffer& p = posArray.emplace_back();
	p.top = topPos;
	p.bottom = bottomPos;
}

void SwordTrail::Update() {
	// �z��̐����擾���w��ȏ�ł����
	// �ŏ��̗v�f������
	if (posArray.size() > KEEP_MAX) {
		// �ŏ��̗v�f���폜
		posArray.erase(posArray.begin());
	}

	// ���_���擾
	vertexCount = posArray.size() * 2;
}

void SwordTrail::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& viewProjection) {
	//�V�F�[�_�[�p�̕ϐ�
	Shader trailShader = Shaders::Ins()->GetTrailShader();
	// �e�V�F�[�_�[�ݒ�
	dc->VSSetShader(trailShader.GetVertexShader().Get(), nullptr, 0);
	dc->PSSetShader(trailShader.GetPixelShader().Get(), nullptr, 0);
	// ���̓��C�A�E�g
	dc->IASetInputLayout(trailShader.GetInputLayout().Get());

	// �����_�[�X�e�[�g�ݒ�
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//blending state object �ݒ�
	blender->setBlendMode(Blender::BLEND_STATE::BS_ALPHA, dc);
	dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
	dc->RSSetState(rasterizerState.Get());

	// �r���[�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX VP = DirectX::XMLoadFloat4x4(&viewProjection);

	// �v���~�e�B�u�ݒ�
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �萔�o�b�t�@�X�V
	CbMesh cbMesh;
	DirectX::XMStoreFloat4x4(&trailCb.data.vp, VP);
	trailCb.applyChanges();
	dc->VSSetConstantBuffers(0, 1, trailCb.GetAddressOf());

	// �g���C���`��
	dc->IASetVertexBuffers(0, 1, trailVertexBuffer.GetAddressOf(), &stride, &offset);



	dc->Draw(vertexCount, 0);
}

void SwordTrail::CreateMesh(ID3D11Device* device) {
	// ���_��
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(vertexCount);

	// �ʒu�ݒ�
	{
		DirectX::XMFLOAT3* p = vertices.get();	
		// �z�񂩂�ʒu���擾
		for (auto& pos : posArray) {
			p->x = pos.top.x;
			p->y = pos.top.y;
			p->z = pos.top.z;
			p++;

			p->x = pos.bottom.x;
			p->y = pos.bottom.y;
			p->z = pos.bottom.z;
			p++;
		}
	}

	// ���_�o�b�t�@
	{
		D3D11_BUFFER_DESC desc{};
		D3D11_SUBRESOURCE_DATA subresourceData{};
		{
			desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * vertexCount);
			desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			subresourceData.pSysMem = vertices.get();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;
		}
		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, trailVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}
}
