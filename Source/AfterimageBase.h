#pragma once
#include "Model.h"
#include "Graphics/Vec.h"
/// 
/// �c���p�I�u�W�F�N�g�̊�{�N���X
/// 
class AfterimageBase
{
public:
    AfterimageBase() {}
    virtual ~AfterimageBase() {}

    // ����.
    virtual void Create() {
        exists = false; // �������͖���.
        // ���ɏ������K�v�ł���΃I�[�o�[���C�h����.
    }

    virtual void Init() {}   // ������.
    virtual void Update(float elapsedTime) {} // �X�V.
    virtual void Render(ID3D11DeviceContext* dc) {} // �`��.

public:
    bool  exists = false;   // �����t���O.
    float   timer = 0;      // �ėp�^�C�}�[.
};
