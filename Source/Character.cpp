#include "Character.h"
#include "Collision.h"
#include "StageManager.h"
#include "Camera/CameraManager.h"

void Character::UpdateTransform()
{
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // ��]�s����쐬
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;


    DirectX::XMMATRIX Q = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
    DirectX::XMMATRIX QS = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternionStans));

    DirectX::XMStoreFloat4x4(&rotaStans, QS);
    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * Q * T;
    // �v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);

    //DirectX::XMVector3Transform()
}

void Character::Grav(float elapsedTime)
{
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    Vec3 grav{};

    // �u���b�N�z�[���i�I�u�W�F�N�g�̒��S�_�j
    int stagecount = StageManager::Instance().GetStageCount();
    for (int i = 0; i < stagecount; i++) {
        Stage* stage = StageManager::Instance().GetStage(i);
        if (stage->GetType() == Stage::Type::Main) {

            Vec3 dir = VecMath::Normalize(stage->GetPos() - position);
            grav = dir * (gravity * elapsedFrame);
            // �ړ�����
            velocity += grav;

            // �@���𔽉f
            normal = -dir;
        }
    }
    // �ő�l����
    if (abs(velocity.x) > velocityMax)
        velocity.x = velocityMax * VecMath::sign(velocity.x);
    if (abs(velocity.y) > velocityMax)
        velocity.y = velocityMax * VecMath::sign(velocity.y);
    if (abs(velocity.z) > velocityMax)
        velocity.z = velocityMax * VecMath::sign(velocity.z);

    // ���C�̊J�n�ʒu�͑������@�������ւ��傢�ړ��i���܂�Ə�������Ȃ��j
    Vec3 start = position + (normal * stepOffset);
    // ��̏I�_�ʒu�͈ړ���
    Vec3 end = position + grav;
    //pos1 = start;
    //pos2 = end;
    // ���C�L���X�g����I�u�W�F�N�g�̒��S�_��
    // �d�͏��������A���̐�փ��C�L���X�g

    // ���C�L���X�g�ɂ��n�ʔ���
    HitResult hit;
    // ���C�̏I�_�ʒu�͈ړ���̈ʒu
    if (StageManager::Instance().RayCast(start, end, hit)) {
        // �n�ʂɐڒn���Ă���
        position = hit.position;

        //pos1 = start;
        //pos2 = end;

        isGround = true;
    }
    else {
        isGround = false;
    }
    if (!isGround) {
        position += velocity * elapsedTime;
    }


    // ���ꂪ�{���������I�I
    // �N�H�[�^�j�I����]�s��쐬
    {
        // �ŏ��Ɏp���i�n�ʂ̖@���ɉ�������]�j   
        // ��x�N�g���Ɩ@���̃x�N�g���Ԃ̊p�x�{�O�ςŉ�]�������߂�
        DirectX::XMVECTOR Up;
        Vec3 n = Vec3(0, 1, 0);
        Up = DirectX::XMLoadFloat3(&n);
        DirectX::XMVECTOR Norm = DirectX::XMLoadFloat3(&normal);
        float stansAng;
        DirectX::XMStoreFloat(&stansAng, DirectX::XMVector3AngleBetweenVectors(Up, Norm));
        DirectX::XMVECTOR QuaternionStans;
        if (stansAng) {
            QuaternionStans = DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Cross(Up, Norm), stansAng);
            DirectX::XMStoreFloat4(&quaternion, QuaternionStans);
            DirectX::XMStoreFloat4(&quaternionStans, QuaternionStans);
        }


        // ���i�K�ł̏�׋�����߂�
        DirectX::XMFLOAT4X4 tran;
        {
            // �X�P�[���s����쐬
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
            DirectX::XMMATRIX Q = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
            // �ʒu�s����쐬
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
            // 3�̍s���g�ݍ��킹�A���[���h�s����쐬
            DirectX::XMMATRIX W = S * Q * T;
            // �v�Z�������[���h�s������o��
            DirectX::XMStoreFloat4x4(&tran, W);
        }

        // ���Ɍ����i�ړ������ւ̉�]�j        
        // �O�x�N�g�������߂āA�i�s�����x�N�g�������߂Ă��̊Ԃ̉�]
        Vec3 front = VecMath::Normalize({ tran._31,tran._32,tran._33 });
        Vec3 right = VecMath::Normalize({ tran._11,tran._12,tran._13 });

        if (posMae == posAto && dir == Vec3()) {
            dir = front;
        }
        else if(posMae != posAto)dir = VecMath::Normalize(VecMath::Subtract(posAto, posMae));
        float dirAng = VecMath::AngleBetweenVectors(front, dir);
        DirectX::XMVECTOR QuaternionDir;
        if (dirAng) {
            //���E����@���l�搶�_
            float dirAng2 = VecMath::AngleBetweenVectors(right, dir);
            if (DirectX::XMConvertToDegrees(dirAng2) > 90) dirAng = -dirAng;

            QuaternionDir = DirectX::XMQuaternionRotationAxis(Norm, dirAng);
        
            //***************
            //�N�H�[�^�j�I��������
            //***************
            if (stansAng) 
                DirectX::XMStoreFloat4(&quaternion, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&quaternion), QuaternionDir));
            else 
                DirectX::XMStoreFloat4(&quaternion, QuaternionDir);
        }


    }
}

void Character::Move(float vx, float vz, float speed)
{
    // �ړ������x�N�g���ݒ�
    moveVecX = vx;
    moveVecZ = vz;

    // �ő呬�x�ݒ�
    maxMoveSpeed = speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;

    // �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    if (vx == 0.0f && vz == 0.0f) return;

    // �i�s�x�N�g����P�ʃx�N�g����
    float length = sqrtf(vx * vx + vz * vz);
    vx /= length;
    vz /= length;

    // ���g�̉�]�l����O���������߂�
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    // ��]�p�����߂邽�߁A2�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (vx * frontX) + (vz * frontZ);

    // ���ϒl��-1.0~1.0�ŕ\������Ă���A2�̒P�ʃx�N�g���̊p�x��
    // �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    float rot = 1.0f - dot;

    // ���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (vx * frontZ) - (vz * frontX);

    // 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
    // ���E������s�����Ƃɂ���č��E�����I������
    if (cross < 0.0f) {
        angle.y -= rot * speed;
    }
    else {
        angle.y += rot * speed;
    }
}

void Character::UpdateSpeed(float elapsedTime)
{
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    // ���͍X�V����
    UpdateVelocity(elapsedFrame);

    // �ړ��X�V����
    UpdateMove(elapsedTime);


    //// �������͍X�V����
    //UpdateVerticalVelocitiy(elapsedFrame);
    //// �������͍X�V����
    //UpdateHorizontalVelocity(elapsedFrame);

    //// �����ړ��X�V����
    //UpdateVerticalMove(elapsedTime);
    //// �����ړ��X�V����
    //UpdateHorizontalMove(elapsedTime);
}

bool Character::ApplyDamage(int damage, float invincibleTime)
{
    // �_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
    if (damage <= 0) return false;

    // ���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
    if (health <= 0) return false;

    if (invincibleTimer > 0) return false;

    invincibleTimer = invincibleTime;

    // �_���[�W����
    health -= damage;

    // ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
    return true;
}

//  ���G���ԍX�V����
void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f) invincibleTimer -= elapsedTime;
}

void Character::UpdateVerticalVelocitiy(float elapsedFrame)
{
    int stageCount = StageManager::Instance().GetStageCount();
    for (int i = 0; i < stageCount; i++) {
        Stage* stage = StageManager::Instance().GetStage(i);
        if (stage->GetType() == Stage::Type::Main) {

            Vec3 dir = VecMath::Normalize((stage->GetPos() + Vec3(0, -150, 0)) - position);
            Vec3 gra = dir * (gravity * elapsedFrame);
            // �ړ�����
            velocity += gra;

            // �@���𔽉f
            normal = -dir;
        }
    }


    // �ړ�����
    //velocity.y += gravity * elapsedFrame;
    // �ő�l����
    if (abs(velocity.x) > velocityMax)
        velocity.x = velocityMax * VecMath::sign(velocity.x);
    if (abs(velocity.y) > velocityMax)
        velocity.y = velocityMax * VecMath::sign(velocity.y);
    if (abs(velocity.z) > velocityMax)
        velocity.z = velocityMax * VecMath::sign(velocity.z);
}

void Character::UpdateVerticalMove(float elapsedTime)
{
    // ���������̈ړ���
    float my = velocity.y * elapsedTime;

    slopeRate = 0.0f;

    //  �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
    //normal = { 0,1,0 };

    // ������
    if (my < 0.0f) {
        // ���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // ���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        // ���C�L���X�g�ɂ��n�ʔ���
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            // �@���x�N�g�����擾
            //normal.x = hit.normal.x;
            //normal.y = hit.normal.y;
            //normal.z = hit.normal.z;

            // �n�ʂɐڒn���Ă���
            position.x = hit.position.x;
            position.y = hit.position.y;
            position.z = hit.position.z;

            // ��]
            angle.y += hit.rotation.y;

            //  �X�Η��̌v�Z
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            // ���n����
            if (!isGround) OnLanding();

            isGround = true;
            velocity.y = 0.0f;
        }
        else {
            // �󒆂ɕ����Ă�
            position.y += my;
            isGround = false;
        }
    }
    // �㏸��
    else if (my > 0.0f) {
        position.y += my;
        isGround = false;
    }


    // �n�ʂ̌����ɉ����悤��XZ����]
    {
        // Y�����@���x�N�g�������Ɍ����I�C���[�p��]���Z�o����
        angle.x = atan2f(normal.z, normal.y);
        angle.z = -atan2f(normal.x, normal.y);
        float ax = atan2f(normal.z, normal.y);
        float az = -atan2f(normal.x, normal.y);
        // ���`�⊮�Ŋ��炩�ɉ�]����
        //angle.x = Mathf::Lerp(angle.x, ax, 0.2f);
        //angle.z = Mathf::Lerp(angle.z, az, 0.2f);
    }

}

// �Ռ���^����
void Character::AddImpulse(const Vec3& impulse)
{
    // ���͂ɗ͂�������
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

#include <math.h>

// �������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedTime)
{
    moveVecX = min(moveVecX, maxMoveSpeed);
    moveVecZ = min(moveVecZ, maxMoveSpeed);


    velocity.x = moveVecX * elapsedTime * maxMoveSpeed;
    velocity.z = moveVecZ * elapsedTime * maxMoveSpeed;

    // �����ŃK�^�K�^���Ȃ��̂��`���Ă���

    //////// XZ���ʂ̑��͂���������
    //float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    //if (length > 0.0f) {
    //    // ���C��
    //    float friction = this->friction * elapsedTime;
    //
    //    // �󒆂ɂ���Ƃ��͖��C�͂����炷
    //    if (!isGround) friction -= airControl;
    //
    //    // ���C�ɂ�鉡�����̑��x����
    //    if (length > friction) {
    //        float vx = velocity.x / length;
    //        float vz = velocity.z / length;
    //
    //        velocity.x -= vx * friction;
    //        velocity.z -= vz * friction;
    //    }
    //    // �������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
    //    else {
    //        velocity.x = 0.0f;
    //        velocity.z = 0.0f;
    //    }
    //}
    //
    //// XZ���ʂ̑��͂���������
    //if (length <= maxMoveSpeed) {
    //    // �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
    //    float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
    //    if (moveVecLength > 0.0f) {
    //        // ������
    //        float acceleration = this->acceleration * elapsedTime;
    //
    //        // �󒆂ɂ���Ƃ��͉����͂����炷
    //        //if (!isGround) acceleration -= airControl;
    //
    //        // �ړ��x�N�g���ɂ���������
    //        velocity.x += acceleration * moveVecX;
    //        velocity.z += acceleration * moveVecZ;
    //
    //        //  �����ŃK�^�K�^���Ȃ��悤�ɂ���
    //        if (isGround && slopeRate > 0.0f) {
    //            velocity.y -= length * slopeRate * elapsedTime;
    //        }
    //
    //        // �ő呬�x����
    //        float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    //        if (length > maxMoveSpeed) {
    //            float vx = velocity.x / length;
    //            float vz = velocity.z / length;
    //
    //            velocity.x = vx * maxMoveSpeed;
    //            velocity.z = vz * maxMoveSpeed;
    //        }
    //    }
    //}
    //�ړ��x�N�g�������Z�b�g
    //moveVecX *= 0.5f;
    //moveVecZ *= 0.5f;
}

// �����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
    /*  �ړ�����
     position.x += velocity.x * elapsedTime;
     position.z += velocity.z * elapsedTime;*/

     // �X���C���͗�����

     // �������͌v�Z
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    //float velocityLengthXZ = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);


    if (velocityLengthXZ > 0.0f) {
        // �����ړ��l
        float mx = velocity.x * elapsedTime;// * speedP;
        float mz = velocity.z * elapsedTime;// * speedP;

        // ���C�̊J�n�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x , position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

        // ���C�L���X�g�ɂ��ǔ���
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            // �ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // �ǂ̖@��
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // ���˃x�N�g����@���Ɏˉe                          // �x�N�g���̔ے肷��
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // �␳�ʒu�̌v�Z                   // v1 �x�N�g���搔 v2 �x�N�^�[��Z   3 �Ԗڂ̃x�N�g���ɒǉ����ꂽ�ŏ��� 2 �̃x�N�g���̐ς��v�Z
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // �߂�l �x�N�g���̐Ϙa��Ԃ�    �߂�l�@���@v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // �␳�ʒu���ǂɖ��܂��Ă��邩�ǂ���
            if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2)) {
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }

        }
        else {
            position.x += mx;
            position.z += mz;
        }
    }
}

void Character::UpdateVelocity(float elapsedFrame)
{
    UpdateVerticalVelocitiy(elapsedFrame);
    UpdateHorizontalVelocity(elapsedFrame);
}

void Character::UpdateMove(float elapsedTime)
{
    UpdateVerticalMove(elapsedTime);
    UpdateHorizontalMove(elapsedTime);
}
