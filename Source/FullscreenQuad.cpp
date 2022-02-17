#include "FullscreenQuad.h"
#include "Graphics/Misc.h"
#include "Graphics/Shaders.h"

FullscreenQuad::FullscreenQuad(ID3D11Device* device)
{	
	//設定
	{
		CD3D11_DEFAULT defaultSettings;
		//デフォルトセッティングで初期化する。
		CD3D11_BLEND_DESC blendDesc(defaultSettings);
		//合成用のブレンドステートを作成する。
		//乗算合成。
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		device->CreateBlendState(&blendDesc, &m_finalBlendState);
	}
}

void FullscreenQuad::Blit(ID3D11DeviceContext* immediateContext,
	ID3D11ShaderResourceView** shader_resource_view, uint32_t start_slot, uint32_t num_views,
	Shader* shader)
{
	//乗算合成用のブレンディングステートを設定する。	
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	immediateContext->OMSetBlendState(m_finalBlendState.Get(), blendFactor, 0xffffffff);

	immediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->IASetInputLayout(nullptr);

	immediateContext->VSSetShader(Shaders::Ins()->GetFullscreenQuadShader().GetVertexShader().Get(), 0, 0);
	shader->GetPixelShader().Get() ? immediateContext->PSSetShader(shader->GetPixelShader().Get(), 0, 0) :
		immediateContext->PSSetShader(Shaders::Ins()->GetFullscreenQuadShader().GetPixelShader().Get(), 0, 0);

	immediateContext->PSSetShaderResources(start_slot, num_views, shader_resource_view);
	immediateContext->VSSetShaderResources(start_slot, num_views, shader_resource_view);


	immediateContext->Draw(4, 0);
}