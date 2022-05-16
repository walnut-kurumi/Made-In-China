#pragma once
#include "AfterimageBase.h"
#include <memory>
#include <vector>

/// <summary>
/// �c���I�u�W�F�N�g�̃v�[�����O�N���X
/// </summary>
template <class T>
class AfterimagePool
{
public:
    AfterimagePool() {}
    virtual ~AfterimagePool() {}

    // ����.
    void Create(int size) {
        members = new T[size];
        this->size = size;
    }

    // �j��.
    void Destroy() {
        delete[] members;
    }

    // ���g�p�̃I�u�W�F�N�g���擾����.
    T* Recycle() {
        for (int i = 0; i < size; i++) {
            // ���g�p�̏ꍇ
            if (!members[i].exists) {
                members[i].exists = true; // �����t���O�𗧂Ă�.
                return &members[i]; // ���g�p�Ȃ̂ōė��p�\.
            }
        }

        // ������Ȃ�����.
        return nullptr;
    }

    // �X�V.
    void Update(float elapsedTime) {
        for (int i = 0; i < size; i++) {
            if (members[i].exists) {
                members[i].Update(elapsedTime); // �������Ă���I�u�W�F�N�g�̂ݍX�V.
            }
        }
    }

    // �`��.
    void Render(ID3D11DeviceContext* dc) {
        for (int i = 0; i < size; i++) {
            if (members[i].exists) {
                members[i].Render(dc); // �������Ă���I�u�W�F�N�g�̂ݕ`��.
            }
        }
    }

    // ���������J�E���g����.
    int Count() {
        int cnt = 0;
        for (int i = 0; i < size; i++) {
            if (members[i].exists) {
                cnt++; // �������Ă���.
            }
        }
        return cnt;
    }

protected:
    T* members{};
    int size = 0;
};
