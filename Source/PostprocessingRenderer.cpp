#include "PostprocessingRenderer.h"
#include "Graphics/Graphics.h"
#include "Framework.h"

PostprocessingRenderer::PostprocessingRenderer()
{
	Graphics& graphics = Graphics::Ins();

	renderSprite = std::make_unique<SpriteResource>();

	// 高輝度抽出用描画ターゲットを生成
	luminanceExtractRenderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth()), static_cast<UINT>(graphics.GetScreenHeight()), DXGI_FORMAT_R8G8B8A8_UNORM);
	// 高輝度抽出ぼかし用描画ターゲットを生成
	luminanceExtractBokehRenderTarget = std::make_unique<RenderTarget>(static_cast<UINT>(graphics.GetScreenWidth()), static_cast<UINT>(graphics.GetScreenHeight()), DXGI_FORMAT_R8G8B8A8_UNORM);
	bloomData.gaussianFilterData.textureSize.x = static_cast<float>(luminanceExtractBokehRenderTarget->GetWidth());
	bloomData.gaussianFilterData.textureSize.y = static_cast<float>(luminanceExtractBokehRenderTarget->GetHeight());

}

PostprocessingRenderer::~PostprocessingRenderer()
{
}

void PostprocessingRenderer::Render(ID3D11DeviceContext* deviceContext)
{
	Graphics& graphics = Graphics::Ins();
	ID3D11Device* device = Graphics::Ins().GetDevice();

	// 現在設定されているバッファを退避して初期化しておく
	UINT			cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView;
	{
		deviceContext->RSGetViewports(&cachedViewportCount, cachedViewports);
		deviceContext->OMGetRenderTargets(1
			, cachedRenderTargetView.ReleaseAndGetAddressOf()
			, cachedDepthStencilView.ReleaseAndGetAddressOf());
	}

	/*RenderContext	rc;
	rc.deviceContext = deviceContext;
	rc.luminanceExtractionData = bloomData.luminanceExtractionData;
	rc.gaussianFilterData = bloomData.gaussianFilterData;*/


	//	高輝度抽出用バッファに描画先を変更して高輝度抽出
	{
		//	描画先を変更
		ID3D11RenderTargetView* rtv = luminanceExtractRenderTarget->GetRenderTargetView().Get();
		FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		deviceContext->ClearRenderTargetView(rtv, color);
		deviceContext->OMSetRenderTargets(1, &rtv, nullptr);
		D3D11_VIEWPORT	viewport{};
		viewport.Width = static_cast<float>(luminanceExtractRenderTarget->GetWidth());
		viewport.Height = static_cast<float>(luminanceExtractRenderTarget->GetHeight());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);

		//	高輝度抽出処理
		lumiExtraShader=std::make_unique<LuminanceExtractionShader>(device);
		lumiExtraShader->Begin(deviceContext);
		//SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);
		//shader->Begin(rc);

		//	描画対象を変更
		renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);
		renderSprite->Update(0, 0, viewport.Width, viewport.Height,
			0, 0, static_cast<float>(sceneData.width), static_cast<float>(sceneData.height),
			0,
			1, 1, 1, 1);
		lumiExtraShader->Draw(deviceContext, renderSprite.get(), bloomData.luminanceExtractionData);
		lumiExtraShader->End(deviceContext);
		//shader->Draw(rc, renderSprite.get());
		//shader->End(rc);
	}

	// 抽出した高輝度描画対象を暈して書き込む
	{
		//	描画先を変更
		ID3D11RenderTargetView* rtv = luminanceExtractBokehRenderTarget->GetRenderTargetView().Get();
		FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		deviceContext->OMSetRenderTargets(1, &rtv, nullptr);
		deviceContext->ClearRenderTargetView(rtv, color);
		D3D11_VIEWPORT	viewport{};
		viewport.Width = static_cast<float>(luminanceExtractBokehRenderTarget->GetWidth());
		viewport.Height = static_cast<float>(luminanceExtractBokehRenderTarget->GetHeight());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);

		//	高輝度抽出処理
		gaussianBlurDhader = std::make_unique<GaussianBlurShader>(device);
		gaussianBlurDhader->Begin(deviceContext);
		//SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
		//shader->Begin(rc);

		//	描画対象を変更
		renderSprite->SetShaderResourceView(luminanceExtractRenderTarget->GetShaderResourceView(),
			luminanceExtractRenderTarget->GetWidth(),
			luminanceExtractRenderTarget->GetHeight());
		renderSprite->Update(0, 0, viewport.Width, viewport.Height,
			0, 0, static_cast<float>(luminanceExtractRenderTarget->GetWidth()), static_cast<float>(luminanceExtractRenderTarget->GetHeight()),
			0,
			1, 1, 1, 1);
		gaussianBlurDhader->Draw(deviceContext, renderSprite.get(), bloomData.gaussianFilterData);
		gaussianBlurDhader->End(deviceContext);
		//shader->Draw(rc, renderSprite.get());
		//shader->End(rc);
	}

	//	元のバッファに戻す
	{
		deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);
		deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
	}

	// 最終パス
	{
		//	高輝度抽出処理
		finalpassshader = std::make_unique<FinalpassShader>(device);
		finalpassshader->Begin(deviceContext);
		//SpriteShader* shader = graphics.GetShader(SpriteShaderId::Finalpass);
		//shader->Begin(rc);


		renderSprite->SetShaderResourceView(sceneData.srv, sceneData.width, sceneData.height);
		renderSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
			0, 0, static_cast<float>(sceneData.width), static_cast<float>(sceneData.height),
			0,
			1, 1, 1, 1);

		//	シェーダーに渡すテクスチャを設定
		finalPass.data.bloomTexture = luminanceExtractBokehRenderTarget->GetShaderResourceView().Get();
		//rc.colorGradingData = colorGradingData;

		finalpassshader->Draw(deviceContext, renderSprite.get(), colorGradingData.data, finalPass.data);
		//shader->Draw(rc, renderSprite.get());
		finalpassshader->End(deviceContext);
		//shader->End(rc);

	}

}

//	シーン情報の設定
void PostprocessingRenderer::SetSceneData(ShaderResourceViewData srvData)
{
	sceneData = srvData;
}

//	デバッグ情報の表示
void PostprocessingRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("PostProcess"))
	{
		if (ImGui::TreeNode("Bloom"))
		{
			ImGui::SliderFloat("threshold", &bloomData.luminanceExtractionData.threshold, 0.0f, 1.0f);
			ImGui::SliderFloat("intensity", &bloomData.luminanceExtractionData.intensity, 0.0f, 10.0f);
			ImGui::SliderInt("kernelSize", &bloomData.gaussianFilterData.kernelSize, 1, 16 - 1);
			ImGui::SliderFloat("deviation", &bloomData.gaussianFilterData.deviation, 1.0f, 10.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ColorGrading"))
		{
			ImGui::SliderFloat("hueShift", &colorGradingData.data.hueShift, 0.0f, 360.0f);
			ImGui::SliderFloat("saturation", &colorGradingData.data.saturation, 0.0f, 2.0f);
			ImGui::SliderFloat("brightness", &colorGradingData.data.brightness, 0.0f, 2.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

}

