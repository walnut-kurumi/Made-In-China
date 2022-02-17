#include "SpriteBatch.h"
#include "Texture.h"
#include "Misc.h"
#include <sstream>



SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites) 
	: maxVertices(maxSprites * 6){
	HRESULT hr = S_OK;

	////頂点情報のセット
	//Vertex vertices[] = {
	//	{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
	//	{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
	//	{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
	//	{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
	//};
	std::unique_ptr<Vertex[]> vertices = std::make_unique<Vertex[]>(maxVertices);

	//頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(maxVertices);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
	}
	D3D11_SUBRESOURCE_DATA subresourceData{};
	{
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
	}
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, this->vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


	//画像ファイルのロード　シェーダリソースビューオブジェクト生成
	loadTextureFromFile(device, filename, this->shaderResourceView.GetAddressOf(), &texture2dDesc);
}


void SpriteBatch::render(ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh){
	render(deviceContext, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void SpriteBatch::render(ID3D11DeviceContext* deviceContext,
	float dx, float dy, float dw, float dh, 
	float r, float g, float b, float a,
	float angle) {
	render(deviceContext, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void SpriteBatch::render(ID3D11DeviceContext* deviceContext,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle,
	float sx, float sy, float sw, float sh){
	HRESULT hr = S_OK;
	//ビューポートのサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &viewport);

	//短径の各頂点の位置を計算
	float ltx = dx, lty = dy;			 //left  - top
	float rtx = dx + dw, rty = dy;		 //right - top
	float lbx = dx, lby = dy + dh;		 //left  - bottom
	float rbx = dx + dw, rby = dy + dh;	 //right - bottom

	//ラムダ式による回転計算
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
	//回転の中心を矩形の中心点にした場合
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	float cos = cosf(DirectX::XMConvertToRadians(angle));
	float sin = sinf(DirectX::XMConvertToRadians(angle));
	//回転計算関数
	//rotate(ltx, lty, cx, cy, angle);
	//rotate(rtx, rty, cx, cy, angle);
	//rotate(lbx, lby, cx, cy, angle);
	//rotate(rbx, rby, cx, cy, angle);
	rotate(ltx, lty, cx, cy, cos, sin);
	rotate(rtx, rty, cx, cy, cos, sin);
	rotate(lbx, lby, cx, cy, cos, sin);
	rotate(rbx, rby, cx, cy, cos, sin);


	//Convert to NDC space
	ltx = 2.0f * ltx / viewport.Width - 1.0f;//x0
	lty = 1.0f - 2.0f * lty / viewport.Height;//y0
	rtx = 2.0f * rtx / viewport.Width - 1.0f;//x1
	rty = 1.0f - 2.0f * rty / viewport.Height;//y1
	lbx = 2.0f * lbx / viewport.Width - 1.0f;//x2
	lby = 1.0f - 2.0f * lby / viewport.Height;//y2
	rbx = 2.0f * rbx / viewport.Width - 1.0f;//x3
	rby = 1.0f - 2.0f * rby / viewport.Height;//y3

	// ltx,lty     rtx,rty
	//       ________
	//      |      ／|
	//      |    ／  |
	//	    |  ／    |
	//      |／______|
	//
	// lbx,lby     rbx,rby


	float u0 = sx / texture2dDesc.Width;
	float v0 = sy / texture2dDesc.Height;
	float u1 = (sx + sw) / texture2dDesc.Width;
	float v1 = (sy + sh) / texture2dDesc.Height;

	vertices.push_back({ { ltx, lty , 0 }, { r, g, b, a }, { u0, v0 } });
	vertices.push_back({ { rtx, rty , 0 }, { r, g, b, a }, { u1, v0 } });
	vertices.push_back({ { lbx, lby , 0 }, { r, g, b, a }, { u0, v1 } });
	vertices.push_back({ { lbx, lby , 0 }, { r, g, b, a }, { u0, v1 } });
	vertices.push_back({ { rtx, rty , 0 }, { r, g, b, a }, { u1, v0 } });
	vertices.push_back({ { rbx, rby , 0 }, { r, g, b, a }, { u1, v1 } });



}

SpriteBatch::~SpriteBatch(){
}

void SpriteBatch::begin(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* shaderResourceView){
	vertices.clear();
	deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
}

void SpriteBatch::begin(ID3D11DeviceContext* deviceContext) {
	begin(deviceContext, this->shaderResourceView.Get());
}

void SpriteBatch::end(ID3D11DeviceContext* deviceContext){
	HRESULT hr = S_OK;

	//計算結果で頂点バッファオブジェクトを更新する
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = deviceContext->Map(this->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

	size_t vertexCount = vertices.size();
	_ASSERT_EXPR(maxVertices >= vertexCount, "Buffer overflow");
	Vertex* data = reinterpret_cast<Vertex*>(mappedSubresource.pData);
	if (data != nullptr) {
		const Vertex* p = vertices.data();
		memcpy_s(data, maxVertices * sizeof(Vertex), p, vertexCount * sizeof(Vertex));
	}
	deviceContext->Unmap(this->vertexBuffer.Get(), 0);

	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	deviceContext->Draw(static_cast<UINT>(vertexCount), 0);
}
