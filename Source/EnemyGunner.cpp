#include "EnemyGunner.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"

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

    isDead = false;

    direction = true;

    TransitionIdleState();
}

void EnemyGunner::Update(float elapsedTime)
{
    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

    UpdateSpeed(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

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


    //// �K�v�Ȃ�����ǉ�
    //debugRenderer.get()->DrawSphere(copos, 4, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos2, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos3, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos4, 1.6f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}

void EnemyGunner::DrawDebugGUI()
{
}


// �p�j  ����true�@false�E��
void EnemyGunner::MoveWalk(bool direction)
{
    float vx;
    (direction ? vx = -1 : vx = 1);
    Move(vx, 0.0f, 5.0f);
}

// �v���C���[�����G
bool EnemyGunner::Search()
{
    return false;
}

// �˒������܂ő���
void EnemyGunner::MoveRun()
{
}

// �˒��������ǂ���
bool EnemyGunner::CheckAttackRange()
{
    return false;
}

// �U��
void EnemyGunner::MoveAttack()
{
}

// �������
void EnemyGunner::MoveBlow()
{
}



// �ҋ@�X�e�[�g�J��
void EnemyGunner::TransitionIdleState()
{
    state = State::Idle;
    model->PlayAnimation(static_cast<int>(state), true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyGunner::UpdateIdleState(float elapsedTime)
{   
    // ���J�E���g���Ă��������ς��Ĉړ��X�e�[�g��
    //direction = true;
    
    //IdleTimerUpdate()

    TransitionWalkState();
}

// �^�[������܂ł̃^�C�}�[�X�V
void EnemyGunner::IdleTimerUpdate()
{
}


// �ړ��X�e�[�g�֑J��
void EnemyGunner::TransitionWalkState()
{
    state = State::Walk;
    moveSpeed = 10;
    model->PlayAnimation(static_cast<int>(state), true);
}

// �ړ��X�e�[�g�X�V����
void EnemyGunner::UpdateWalkState(float elapsedTime)
{   
    // �p�j
    MoveWalk(direction);

    // �v���C���[���������瑖��
    if (Search())
    {
        TransitionRunState();
    }
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
    // �˒���������܂Ńv���C���[�Ɍ������đ��葱����
    MoveRun();
    // �˒��������Ȃ�U���X�e�[�g��
    if (CheckAttackRange())
    {
        TransitionAttackState();
    }
}


//�U���X�e�[�g�J��
void EnemyGunner::TransitionAttackState()
{
    state = State::Attack;    
    model->PlayAnimation(static_cast<int>(state), true);
}

//�U���X�e�[�g�X�V����
void EnemyGunner::UpdateAttackState(float elapsedTime)
{
    MoveAttack();

    // �˒������O�Ȃ瑖��X�e�[�g��
    if (!CheckAttackRange())
    {
        TransitionRunState();
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