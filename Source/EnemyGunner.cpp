#include "EnemyGunner.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"
#include "Camera/CameraManager.h"
#include "EnemyBulletStraight.h"

#include "HitManager.h"

EnemyGunner::EnemyGunner(ID3D11Device* device)
{
    const char* idle = "Data/Models/Enemy/Animations/Idle.fbx";
    const char* run = "Data/Models/Enemy/Animations/Running.fbx";
    const char* walk = "Data/Models/Enemy/Animations/Walking.fbx";
    const char* attack = "Data/Models/Enemy/Animations/Attack.fbx";
    const char* blow = "Data/Models/Enemy/Animations/GetHit1.fbx";
    const char* death = "Data/Models/Enemy/Animations/Death.fbx";


    model = new Model(device, "Data/Models/Enemy/Jummo.fbx", true, 0);

    model->LoadAnimation(idle, 0, static_cast<int>(State::Idle));
    model->LoadAnimation(run, 0, static_cast<int>(State::Run));
    model->LoadAnimation(walk, 0, static_cast<int>(State::Walk));
    model->LoadAnimation(attack, 0, static_cast<int>(State::Attack));
    model->LoadAnimation(blow, 0, static_cast<int>(State::Blow));
    model->LoadAnimation(death, 0, static_cast<int>(State::Death));

    UpdateState[static_cast<int>(State::Idle)] = &EnemyGunner::UpdateIdleState;
    UpdateState[static_cast<int>(State::Run)] = &EnemyGunner::UpdateRunState;
    UpdateState[static_cast<int>(State::Walk)] = &EnemyGunner::UpdateWalkState;
    UpdateState[static_cast<int>(State::Attack)] = &EnemyGunner::UpdateAttackState;
    UpdateState[static_cast<int>(State::Blow)] = &EnemyGunner::UpdateBlowState;
    UpdateState[static_cast<int>(State::Death)] = &EnemyGunner::UpdateDeathState;

    position = { 0.0f, 0.0f, 0.0f };    

    scale = { 0.05f, 0.05f, 0.05f };
       
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

EnemyGunner::~EnemyGunner()
{
}

void EnemyGunner::Init()
{   
    posMae = position;
    posAto = position;
    angle = { 0,0,0 };
    transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    normal = { 0,0,0 };
    velocity = { 0,0,0 };
    moveSpeed = 10;

    moveVecX = 0.0f;
    moveVecZ = 0.0f;
    health = 40;

    centerPosition = position;
    centerPosition.y += 1.0f;
    radius = 2.5;

    isDead = false;

    direction = true;

    TransitionAttackState();
}

void EnemyGunner::Update(float elapsedTime)
{
    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

    UpdateSpeed(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // �e�ۍX�V����
    bulletManager.Update(elapsedTime);

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();
    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);
    //���f���s��X�V
    model->UpdateTransform(transform);
}

void EnemyGunner::Render(ID3D11DeviceContext* dc,Shader* shader)
{
    model->Begin(dc, *shader);    
    model->Render(dc);

    // �e�ە`�揈��
    bulletManager.Render(dc, shader);

    // ���S���W�X�V
    centerPosition = position;
    centerPosition.y += height;

    //// �K�v�Ȃ�����ǉ�
    debugRenderer.get()->DrawSphere(centerPosition, radius, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos2, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos3, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos4, 1.6f, Vec4(1, 0, 0, 1));
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}



// �p�j  ����true�@false�E��
void EnemyGunner::MoveWalk(bool direction)
{
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f,moveSpeed);
}

// �v���C���[�����G
bool EnemyGunner::Search()
{
    return false;
}

// �˒������܂ő���
void EnemyGunner::MoveRun(bool direction)
{
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f, moveSpeed);
}

// �˒��������ǂ���
bool EnemyGunner::CheckAttackRange()
{
    return false;
}

// �U��
void EnemyGunner::MoveAttack()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();
    
    // ���i�e�۔���   
    {
        float vx;
        (direction ? vx = -1 : vx = 1);
        // ���˂������
        DirectX::XMFLOAT3 dir;
        dir.x = vx;
        dir.y = 0.0f;
        dir.z = 0.0f;       
        // ����
        EnemyBulletStraight* bullet = new EnemyBulletStraight(device, &bulletManager);
        bullet->Launch(dir, centerPosition);

        // �N�[���_�E���ݒ�
        attackCooldown = 90;
    }
}

// �������
void EnemyGunner::MoveBlow()
{
}



// �ҋ@�X�e�[�g�J��
void EnemyGunner::TransitionIdleState()
{
    state = State::Idle;
    turnFlag = false;
    walkFlag = false;
    idleTimer = 120;
    moveSpeed = 0;
    model->PlayAnimation(static_cast<int>(state), true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyGunner::UpdateIdleState(float elapsedTime)
{   
    Move(0.0f, 0.0f, moveSpeed);
    IdleTimerUpdate(elapsedTime);

    if (walkFlag)
    {
        // ���J�E���g���Ă��������ς��Ĉړ��X�e�[�g��
        direction = !direction;
        // �����X�e�[�g�ֈړ�
        TransitionWalkState();
    }
}

// �^�[������܂ł̃^�C�}�[�X�V
void EnemyGunner::IdleTimerUpdate(float elapsedTime)
{
    if (idleTimer > 0)idleTimer-= elapsedTime;
    else walkFlag = true;
}


// �ړ��X�e�[�g�֑J��
void EnemyGunner::TransitionWalkState()
{
    state = State::Walk;
    turnFlag = false;
    walkFlag = true;
    walkTimer = 120;
    moveSpeed = 5;
    model->PlayAnimation(static_cast<int>(state), true);
}

// �ړ��X�e�[�g�X�V����
void EnemyGunner::UpdateWalkState(float elapsedTime)
{   
    // �p�j
    if (walkFlag)
    {
        MoveWalk(direction);
        WalkTimerUpdate(elapsedTime);
    }

    if (turnFlag)
    {
        TransitionIdleState();        
    }


    // �v���C���[���������瑖��
    if (Search())
    {
        TransitionRunState();
    }
}

// �~�܂�܂ł̃^�C�}�[�X�V����
void EnemyGunner::WalkTimerUpdate(float elapsedTime)
{
    if (walkTimer > 0)walkTimer-= elapsedTime;
    else turnFlag = true;
}


//����X�e�[�g�J��
void EnemyGunner::TransitionRunState()
{
    state = State::Run;
    moveSpeed = 15;
    model->PlayAnimation(static_cast<int>(state), true);
}

//����X�e�[�g�X�V����
void EnemyGunner::UpdateRunState(float elapsedTime)
{    
    // �˒��������Ȃ�U���X�e�[�g��
    if (CheckAttackRange())
    {
        TransitionAttackState();
    }
    // �˒���������܂Ńv���C���[�Ɍ������đ��葱����
    MoveRun(direction);
}


//�U���X�e�[�g�J��
void EnemyGunner::TransitionAttackState()
{
    state = State::Attack; 
    moveSpeed = 0;
    attackCooldown = 0;
    model->PlayAnimation(static_cast<int>(state), true);
}

//�U���X�e�[�g�X�V����
void EnemyGunner::UpdateAttackState(float elapsedTime)
{
    // �~�܂�
    Move(0.0f, 0.0f, moveSpeed);
    // �U��
    attackCooldown -= elapsedTime;    
    if(attackCooldown <= 0.0f)
    {
        MoveAttack();
    }


    // �˒������O�Ȃ瑖��X�e�[�g��
    if (!CheckAttackRange())
    {
        //TransitionRunState();
    }
}


//������уX�e�[�g�J��
void EnemyGunner::TransitionBlowState()
{
    state = State::Blow;   
    model->PlayAnimation(static_cast<int>(state), true);
}

//������уX�e�[�g�X�V����
void EnemyGunner::UpdateBlowState(float elapsedTime)
{
    // ������΂�
    MoveBlow();

    // ������΂����玀�S�X�e�[�g��
    TransitionDeathState();
}


//���S�X�e�[�g�J��
void EnemyGunner::TransitionDeathState()
{
    state = State::Death;
    model->PlayAnimation(static_cast<int>(state), true);
}

//���S�X�e�[�g�X�V����
void EnemyGunner::UpdateDeathState(float elapsedTime)
{
   // ���S�A�j���[�V�����I���������ł�����
    OnDead();
}