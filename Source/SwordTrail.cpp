#include "SwordTrail.h"
#include "Graphics/Misc.h"
#include "Graphics/Shaders.h"

SwordTrail::SwordTrail(ID3D11Device* device, ID3D11DeviceContext* dc) {
	// 定数バッファ
	trailCb.initialize(device, dc);

	//ブレンディングステート
	blender = std::make_unique<Blender>(device);

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ラスタライザーステート
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
	// 配列の数を取得し指定以上であれば
	// 最初の要素を消す
	if (posArray.size() > KEEP_MAX) {
		// 最初の要素を削除
		posArray.erase(posArray.begin());
	}

	// 頂点数取得
	vertexCount = posArray.size() * 2;
}

void SwordTrail::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& viewProjection) {
	//シェーダー用の変数
	Shader trailShader = Shaders::Ins()->GetTrailShader();
	// 各シェーダー設定
	dc->VSSetShader(trailShader.GetVertexShader().Get(), nullptr, 0);
	dc->PSSetShader(trailShader.GetPixelShader().Get(), nullptr, 0);
	// 入力レイアウト
	dc->IASetInputLayout(trailShader.GetInputLayout().Get());

	// レンダーステート設定
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//blending state object 設定
	blender->setBlendMode(Blender::BLEND_STATE::BS_ALPHA, dc);
	dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
	dc->RSSetState(rasterizerState.Get());

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX VP = DirectX::XMLoadFloat4x4(&viewProjection);

	// プリミティブ設定
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 定数バッファ更新
	CbMesh cbMesh;
	DirectX::XMStoreFloat4x4(&trailCb.data.vp, VP);
	trailCb.applyChanges();
	dc->VSSetConstantBuffers(0, 1, trailCb.GetAddressOf());

	// トレイル描画
	dc->IASetVertexBuffers(0, 1, trailVertexBuffer.GetAddressOf(), &stride, &offset);



	dc->Draw(vertexCount, 0);
}

void SwordTrail::CreateMesh(ID3D11Device* device) {
	// 頂点数
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(vertexCount);

	// 位置設定
	{
		DirectX::XMFLOAT3* p = vertices.get();	
		// 配列から位置を取得
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

	// 頂点バッファ
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
