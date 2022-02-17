#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include "Graphics/Shader.h"

class FullscreenQuad
{
public:
	FullscreenQuad(ID3D11Device* device);
	virtual ~FullscreenQuad() = default;

	Microsoft::WRL::ComPtr<ID3D11BlendState> m_finalBlendState;        //乗算合成用のブレンディングステート。

public:
	void Blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView** shader_resource_view,
		uint32_t start_slot, uint32_t num_views, Shader* shader = nullptr);
}; 
