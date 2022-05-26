#pragma once
#include "AfterimageBase.h"
#include "AfterimagePool.h"
#include "Graphics/SkinnedMesh.h"

class AfterimageSkinnedMesh : public AfterimageBase
{
public:
    AfterimageSkinnedMesh() {}
    virtual ~AfterimageSkinnedMesh() {}

    // 初期化.
    void Init() override;
    // 更新
    void Update(float elapsedTime) override;
    // 描画
    void Render(ID3D11DeviceContext* dc) override;
    // 親の描画に必要なデータを取得
    void SetParentData(
        std::vector<SkinnedMesh::Mesh> meshes,
        std::unordered_map<uint64_t, SkinnedMesh::Material> materials,
        SkinnedMesh::Constants cb,
        DirectX::XMFLOAT4X4 transform
    );
    
private:
    // 持ってきた描画用データ
    std::vector<SkinnedMesh::Mesh> meshes;
    std::unordered_map<uint64_t, SkinnedMesh::Material> materials;
    SkinnedMesh::Constants cb;
    DirectX::XMFLOAT4X4 transform; // 位置

    // コンスタントバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    // 残像の生存時間
    float afterImageLifeTime = 0.06f;
    // 徐々に透明度を上げてゆく
    float alpha = 1.0f;

};

class AfterimageSkinedMeshPool : public AfterimagePool<AfterimageSkinnedMesh>
{
};