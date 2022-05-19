#pragma once

#include "AfterimageSkinnedMesh.h"

/// <summary>
/// �c���̑���p�I�u�W�F�N�g
/// </summary>
class AfterimageManager
{
private:
    AfterimageManager() = default;
    ~AfterimageManager() = default;
    static AfterimageManager* instance;

public:
    // �B��̃C���X�^���X�擾
    static AfterimageManager& Instance() {
        static AfterimageManager instance;
        return instance;
    }
private:
    // �c���v�[��
    AfterimageSkinedMeshPool pool;
    // ����������c���I�u�W�F�N�g
    std::vector<AfterimageSkinnedMesh*> afterimage{};
    // �c���̐e�I�u�W�F�N�g
    Model* parent{};
    // ���O������
    int preLoadCount = 10;
    // �c���̐����Ԋu
    float createIntervalTime = 0.01f;
    // �����^�C�}�[
    float createTimer = 0.0f;

public:

    // �C�j�V�����C�Y
    void Initialise();
    // �X�V
    void Update(float elapsedTime);
    // �`��
    void Render(ID3D11DeviceContext* dc);
    // �I������
    void Destroy();

    // �e�̕`��ɕK�v�ȃf�[�^���擾
    void SetParentData(
        std::vector<SkinnedMesh::Mesh> meshes,
        std::unordered_map<uint64_t, SkinnedMesh::Material> materials,
        SkinnedMesh::Constants cb,
        DirectX::XMFLOAT4X4 transform
    );

    std::vector<SkinnedMesh::Mesh> meshes;
    std::unordered_map<uint64_t, SkinnedMesh::Material> materials;
    SkinnedMesh::Constants cb;
    DirectX::XMFLOAT4X4 transform; // �ʒu
};