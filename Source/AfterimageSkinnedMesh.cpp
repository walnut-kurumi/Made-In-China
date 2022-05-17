#include "AfterimageSkinnedMesh.h"
#include "Graphics/Graphics.h"
void AfterimageSkinnedMesh::Init() {
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(SkinnedMesh::Constants);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	}
	hr = Graphics::Ins().GetDevice()->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


	// 親の初期化
	timer = 0.0f;      // 汎用タイマー.
}

void AfterimageSkinnedMesh::Update(float elapsedTime) {
	timer += elapsedTime;

	// 一定時間過ぎたら消す
	if (timer >= afterImageLifeTime) {
		exists = false;
	}
}

void AfterimageSkinnedMesh::Render(ID3D11DeviceContext* dc) {
	using namespace DirectX;
	// プリミティブモード設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cb.world = transform;
	// 位置をずらして無理やりプレイヤーにかぶらないようにする
	cb.world._43 = -3;

	for (const SkinnedMesh::Mesh& mesh : meshes) {
		uint32_t stride = sizeof(SkinnedMesh::Vertex);
		uint32_t offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//サブセットごとに描画
		for (const SkinnedMesh::Subset& subset : mesh.subsets) {
			const SkinnedMesh::Material& material = materials.at(subset.materialUniqueId);
			//materialColorはdiffuseを入れる(render引数で各色の濃淡を調節)
			DirectX::XMFLOAT4 materialColor = { 0.0f,0.5f,0.5f,0.2f };
			XMStoreFloat4(&cb.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));
			dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cb, 0, 0);
			dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			dc->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
			dc->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());

			dc->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}
	}
}

void AfterimageSkinnedMesh::SetParentData(
	std::vector<SkinnedMesh::Mesh> meshes, 
	std::unordered_map<uint64_t, SkinnedMesh::Material> materials,
	SkinnedMesh::Constants cb, 
	DirectX::XMFLOAT4X4 transform
) {
	this->meshes = meshes;
	this->materials = materials;
	this->cb = cb;
	this->transform = transform;
}