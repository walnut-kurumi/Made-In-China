#pragma once
#include "AfterimageBase.h"
#include "AfterimagePool.h"
#include "Graphics/SkinnedMesh.h"

class AfterimageSkinnedMesh : public AfterimageBase
{
public:
    AfterimageSkinnedMesh() {}
    virtual ~AfterimageSkinnedMesh() {}

    // ������.
    void Init() override;
    // �X�V
    void Update(float elapsedTime) override;
    // �`��
    void Render(ID3D11DeviceContext* dc) override;
    // �e�̕`��ɕK�v�ȃf�[�^���擾
    void SetParentData(
        std::vector<SkinnedMesh::Mesh> meshes,
        std::unordered_map<uint64_t, SkinnedMesh::Material> materials,
        SkinnedMesh::Constants cb,
        DirectX::XMFLOAT4X4 transform
    );
    
private:
    // �����Ă����`��p�f�[�^
    std::vector<SkinnedMesh::Mesh> meshes;
    std::unordered_map<uint64_t, SkinnedMesh::Material> materials;
    SkinnedMesh::Constants cb;
    DirectX::XMFLOAT4X4 transform; // �ʒu

    // �R���X�^���g�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    // �c���̐�������
    float afterImageLifeTime = 0.06f;
    // ���X�ɓ����x���グ�Ă䂭
    float alpha = 1.0f;

};

class AfterimageSkinedMeshPool : public AfterimagePool<AfterimageSkinnedMesh>
{
};