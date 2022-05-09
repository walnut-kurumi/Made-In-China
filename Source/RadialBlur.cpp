#include "RadialBlur.h"

RadialBlur::RadialBlur(ID3D11Device* device) {

	// RadialBlurシェーダ
	{
		embeddedShader.InitCSO(device, nullptr, 0,
			"shader\\obj\\RadialBlurVs.cso", "shader\\obj\\RadialBlurPs.cso"
		);
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

	deviceContext->Draw(4, 0);


}