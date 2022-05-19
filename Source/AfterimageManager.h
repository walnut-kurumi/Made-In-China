#pragma once

#include "AfterimageSkinnedMesh.h"

/// <summary>
/// 残像の操作用オブジェクト
/// </summary>
class AfterimageManager
{
private:
    AfterimageManager() = default;
    ~AfterimageManager() = default;
    static AfterimageManager* instance;

public:
    // 唯一のインスタンス取得
    static AfterimageManager& Instance() {
        static AfterimageManager instance;
        return instance;
    }
private:
    // 残像プール
    AfterimageSkinedMeshPool pool;
    // 発生させる残像オブジェクト
    std::vector<AfterimageSkinnedMesh*> afterimage{};
    // 残像の親オブジェクト
    Model* parent{};
    // 事前生成数
    int preLoadCount = 10;
    // 残像の生成間隔
    float createIntervalTime = 0.01f;
    // 生成タイマー
    float createTimer = 0.0f;

public:

    // イニシャライズ
    void Initialise();
    // 更新
    void Update(float elapsedTime);
    // 描画
    void Render(ID3D11DeviceContext* dc);
    // 終了処理
    void Destroy();

    // 親の描画に必要なデータを取得
    void SetParentData(
        std::vector<SkinnedMesh::Mesh> meshes,
        std::unordered_map<uint64_t, SkinnedMesh::Material> materials,
        SkinnedMesh::Constants cb,
        DirectX::XMFLOAT4X4 transform
    );

    std::vector<SkinnedMesh::Mesh> meshes;
    std::unordered_map<uint64_t, SkinnedMesh::Material> materials;
    SkinnedMesh::Constants cb;
    DirectX::XMFLOAT4X4 transform; // 位置
};