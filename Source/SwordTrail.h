#pragma once
#include <vector>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include "Graphics/Vec.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Blender.h"
#include "Graphics/Shader.h"

class SwordTrail
{
public:
	SwordTrail(ID3D11Device* device, ID3D11DeviceContext* dc);
	~SwordTrail() {}

	void SetSwordPos(const Vec3& topPos, const Vec3& bottomPos);
	void Update();
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& viewProjection);

	// ÉÅÉbÉVÉÖçÏê¨
	void CreateMesh(ID3D11Device* device);

private:
	struct TrailBuffer
	{
		Vec3 top = { 0,0,0 };
		Vec3 bottom = { 0,0,0 };
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	vp;
	};

private:
	std::vector<TrailBuffer> posArray;
	

	static const size_t KEEP_MAX = 180;
	int vertexCount = 0.0f;

	Microsoft::WRL::ComPtr<ID3D11Buffer> trailVertexBuffer;

	std::unique_ptr<Blender> blender;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	ConstantBuffer<CbMesh> trailCb;


};