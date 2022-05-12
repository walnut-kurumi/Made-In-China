#include "RadialBlur.h"
#include "Graphics/Misc.h"

RadialBlur::RadialBlur(ID3D11Device* device) {
	HRESULT hr{ S_OK };
	// RadialBlurシェーダ
	{
		embeddedShader.InitCSO(device, nullptr, 0,
			"shader\\obj\\RadialBlurVs.cso", "shader\\obj\\RadialBlurPs.cso"
		);
	}
	// ラスタライザーステート
	{
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		{
			hr = device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		}
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}
	// 深度ステンシルステート
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
}

void RadialBlur::blit(
	ID3D11DeviceContext* deviceContext,
	ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot,
	uint32_t numViews,
	ID3D11PixelShader* replacedPixelShader) {


	deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(nullptr);

	deviceContext->VSSetShader(embeddedShader.GetVertexShader().Get(), 0, 0);
	replacedPixelShader ? deviceContext->PSSetShader(replacedPixelShader, 0, 0) : deviceContext->PSSetShader(embeddedShader.GetPixelShader().Get(), 0, 0);

	deviceContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	//ラスタライザステート設定
	deviceContext->RSSetState(rasterizerState.Get());
	//深度ステンシルステートオブジェクトを設定
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);

	deviceContext->Draw(4, 0);


}