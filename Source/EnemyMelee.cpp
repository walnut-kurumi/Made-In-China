#include "EnemyMelee.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"
#include "Camera/CameraManager.h"

#include "HitManager.h"

EnemyMelee::EnemyMelee(ID3D11Device* device)
{

    const char* idle = "Data/Models/Enemy/Animations/Idle.fbx";
    const char* run = "Data/Models/Enemy/Animations/Running.fbx";
    const char* walk = "Data/Models/Enemy/Animations/Walking.fbx";
    const char* attack = "Data/Models/Enemy/Animations/Attack.fbx";
    const char* blow = "Data/Models/Enemy/Animations/GetHit1.fbx";
    const char* death = "Data/Models/Enemy/Animations/Death.fbx";

    model = new Model(device, "Data/Models/Enemy/Jummo.fbx");

    model->LoadAnimation(idle, 0, static_cast<int>(State::Idle));
    model->LoadAnimation(run, 0, static_cast<int>(State::Run));
    model->LoadAnimation(walk, 0, static_cast<int>(State::Walk));
    model->LoadAnimation(attack, 0, static_cast<int>(State::Attack));
    model->LoadAnimation(blow, 0, static_cast<int>(State::Blow));
    model->LoadAnimation(death, 0, static_cast<int>(State::Death));

    UpdateState[static_cast<int>(State::Idle)] = &EnemyMelee::UpdateIdleState;
    UpdateState[static_cast<int>(State::Run)] = &EnemyMelee::UpdateRunState;
    UpdateState[static_cast<int>(State::Walk)] = &EnemyMelee::UpdateWalkState;
    UpdateState[static_cast<int>(State::Attack)] = &EnemyMelee::UpdateAttackState;
    UpdateState[static_cast<int>(State::Blow)] = &EnemyMelee::UpdateBlowState;
    UpdateState[static_cast<int>(State::Death)] = &EnemyMelee::UpdateDeathState;

    position = { 0.0f, 0.0f, 0.0f };

    scale = { 0.05f, 0.05f, 0.05f };

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

EnemyMelee::~EnemyMelee()
{
    delete model;
    isAttack = false;
}

void EnemyMelee::Init()
{
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
    health = 1;

    materialColor = { 1,1,1,1 };

    height = 6.0f;

    centerPosition = position;
    centerPosition.y += 1.0f;
    radius = 2.5;

    isDead = false;

    direction = false;

    isAttack = false;
    isSearch = false;

    TransitionWalkState();
}

void EnemyMelee::Update(float elapsedTime)
{
    position.z = 0;

    if (isDead)return;

    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

    // ���S���W�X�V
    UpdateCenterPosition();

    // ���˂����e�ۂƂ̏Փ˔���
    CollisionProjectileVsEnemies();
    // �U���ƃv���C���[�̔���
    if(isAttack)CollisionPanchiVsPlayer();
    
    // ���x�X�V
    UpdateSpeed(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // ���G�G���A�X�V
    UpdateSearchArea();

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();
    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);
    //���f���s��X�V
    model->UpdateTransform(transform);

    FallIsDead();
}

void EnemyMelee::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    if (isDead == false)
    {
        switch (groupNum)
        {
        case 0:
            materialColor = { 0,1,0,1 };
            break;
        case 1:
            materialColor = { 0,0,1,1 };
            break;
        case 2:
            materialColor = { 0,1,1,1 };
            break;

        default:
            break;
        }
        model->Begin(dc, *shader);
        model->Render(dc, materialColor);

      
#ifdef _DEBUG
        // height
        Vec3 heightPos = position;
        heightPos.y += height;

        // DEBUG        
        if(isAttack)debugRenderer.get()->DrawSphere(attackPos, attackRadius, Vec4(0.5f, 1, 0.5f, 1));

        debugRenderer.get()->DrawSphere(heightPos, 1, Vec4(0.5f, 1, 0, 1));
        debugRenderer.get()->DrawSphere(centerPosition, radius, Vec4(1, 0, 0, 1));

        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x, searchAreaPos.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));
        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x + searchAreaScale.x, searchAreaPos.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));
        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x, searchAreaPos.y + searchAreaScale.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));
        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x + searchAreaScale.x, searchAreaPos.y + searchAreaScale.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));

        debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
#endif
    }
}


// ���˂����e�ۂƂ̏Փ˔���
void EnemyMelee::CollisionProjectileVsEnemies()
{
    EnemyBulletManager& enemyBManager = EnemyBulletManager::Instance();
    int enemyBCount = enemyBManager.GetProjectileCount();
    for (int i = 0; i < enemyBCount; ++i)
    {
        EnemyBullet* enemyB = enemyBManager.GetProjectile(i);
        // �Փˏ���
        if (Collision::SphereVsSphere(enemyB->GetPosition(), centerPosition, enemyB->GetRadius(), radius))
        {
            // ���˂��Ă���
            if (enemyB->GetReflectionFlag())
            {
                // �_���[�W�^����
                if (health > 0)
                {
                    ApplyDamage(1, 0.0f);
                    /*if (player->GetSlowFlag() == false)*/
                    player->SetHitstop(true);
                }

                enemyBManager.Remove(enemyB);
            }
        }
    }
}

// �U���ƃv���C���[�̔���
void EnemyMelee::CollisionPanchiVsPlayer()
{
    // �Փˏ���
    if (Collision::SphereVsSphere(attackPos, player->GetCenterPosition(), attackRadius, player->GetRadius())) {
        // �_���[�W�^����
        player->ApplyDamage(1, 0.8f);
    }
}

// �p�j  ����true�@false�E��
void EnemyMelee::MoveWalk(bool direction)
{
    // walk
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f, moveSpeed);
}

// ���G�G���A�X�V
void EnemyMelee::UpdateSearchArea()
{
    if (!direction)
    {
        searchAreaPos = { position.x - 10, position.y - 2.0f };
        searchAreaScale = { 45, height + 5.0f };
    }
    else
    {
        searchAreaPos = { position.x - 35, position.y - 2.0f };
        searchAreaScale = { 45, height + 5.0f };
    }
    // �~�܂��Ă���Ƃ��͗����݂��
    if (!walk)
    {
        searchAreaPos = { position.x - 25, position.y - 2.0f };
        searchAreaScale = { 50, height + 5.0f };
    }
}

// ���S���W
void EnemyMelee::UpdateCenterPosition()
{
    // ���S���W�X�V
    centerPosition = position;
    centerPosition.y += height / 2.0f;
}

// �v���C���[�����G
bool EnemyMelee::Search()
{
    // isSearch = true �Ȃ�return true
    if (isSearch)return true;

    //searchArea�i�Z�`�j �� playerPos�i�_�j�œ����蔻��    
    //�������Ă�������G�͈͓��Ȃ̂�true
    if (Collision::PointVsRect(Vec2(player->GetCenterPosition().x, player->GetCenterPosition().y), searchAreaPos, searchAreaScale))
    {
        if (player->GetCenterPosition().x > position.x)direction = false;
        else if (player->GetCenterPosition().x < position.x) direction = true;

        materialColor = { 1.0f,0.25f,0.25f,1.0f };

        isSearch = true;

        return true;
    }
    //�������Ă��Ȃ����false    
    return false;
}

// �˒������܂ő���
void EnemyMelee::MoveRun(bool direction)
{
    // �������v���C���[�̕�����
    if (player->GetCenterPosition().x > position.x)direction = false;
    else if (player->GetCenterPosition().x < position.x) direction = true;

    // run
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f, moveSpeed);
}

// �˒��������ǂ���
bool EnemyMelee::CheckAttackRange()
{
    // �v���C���[�̍��W�ƃK���i�[�̍��W�Ńx�N�g���쐬
    Vec3 distance = { player->GetCenterPosition() - position };
    //�@�x�N�g�����狗���擾    
    float range = VecMath::LengthVec3(distance);
    // �˒�������菬�����Ȃ�true
    if (attackRange >= range)return true;
    // �˒��������傫���Ȃ�false
    else return false;
}

// �U��
void EnemyMelee::MoveAttack(float cooldown)
{
    if (attackCooldown > 0.0f)
    {
        isAttack = true;
        return;
    }
    // �U���ӂ炮
    isAttack = true;

    // �U������������v���C���[�̕�����
    if (player->GetCenterPosition().x > position.x)direction = false;
    else if (player->GetCenterPosition().x < position.x) direction = true;

    // �N�[���_�E���ݒ�
    attackCooldown = cooldown;

    ID3D11Device* device = Graphics::Ins().GetDevice();

    // ���i�e�۔���   
    {
        // �̂̌���
        float vx;
        (direction ? vx = -1 : vx = 1);
        angle.y = DirectX::XMConvertToRadians(90 * vx);

        // �U���A�j���[�V�����Đ�
        model->PlayAnimation(static_cast<int>(state), false);
        
        // �v���C���[�̒��S���W
        const Vec3& p = { player->GetCenterPosition() };
        // �G�l�~�[�̒��S���W
        const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };

        // ���˂������       
        // �v���C���[�Ɍ�������
        Vec3 pe = { p - e };
        VecMath::Normalize(pe);
        attackPos = centerPosition + pe;
        pe *= 0.1f;
               
        isAttack = false;
    }
}

// �������
void EnemyMelee::MoveBlow()
{
    // �v���C���[�̒��S���W
    const Vec3& p = { player->GetCenterPosition() };
    // �G�l�~�[�̒��S���W
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };

    float vx = e.x - p.x;
    float vy = e.y - p.y;
    float lengthXY = sqrtf(vx * vx + vy * vy);
    vx /= lengthXY;
    vy /= lengthXY;

    // ������΂�    100.0f = �Ռ��̋���
    AddImpulse(Vec3(vx, vy, 0) * 100.0f);

}

//�ː����ʂ��Ă��Ȃ���
bool EnemyMelee::AttackRayCheck()
{
    // �v���C���[�̒��S���W
    const Vec3& p = { player->GetCenterPosition() };
    // �G�l�~�[�̒��S���W
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };


    // �v���C���[�����փ��C�L���X�g
    HitResult hit;

    return StageManager::Instance().RayCast(e, p, hit);

}



// �ҋ@�X�e�[�g�J��
void EnemyMelee::TransitionIdleState()
{
    state = State::Idle;
    turnFlag = false;
    walkFlag = false;
    isSearch = false;
    idleTimer = 5.0f;
    moveSpeed = 0;
    model->PlayAnimation(static_cast<int>(state), true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyMelee::UpdateIdleState(float elapsedTime)
{
    // ����ł��� ������уX�e�[�g��
    if (health <= 0) TransitionBlowState();

    Move(0.0f, 0.0f, moveSpeed);
    IdleTimerUpdate(elapsedTime);

    if (walkFlag)
    {
        // ���J�E���g���Ă��������ς��Ĉړ��X�e�[�g��
        direction = !direction;
        // �����X�e�[�g�ֈړ�
        TransitionWalkState();
    }

    // �v���C���[���������瑖��
    if (Search())
    {
        TransitionRunState();
    }
}

// �^�[������܂ł̃^�C�}�[�X�V
void EnemyMelee::IdleTimerUpdate(float elapsedTime)
{
    if (idleTimer > 0.0f)idleTimer -= elapsedTime;
    else walkFlag = true;
}


// �ړ��X�e�[�g�֑J��
void EnemyMelee::TransitionWalkState()
{
    state = State::Walk;
    turnFlag = false;
    walkFlag = true;
    isSearch = false;
    walkTimer = 5.0f;
    moveSpeed = 5;
    model->PlayAnimation(static_cast<int>(state), true);
}

// �ړ��X�e�[�g�X�V����
void EnemyMelee::UpdateWalkState(float elapsedTime)
{
    // �������Ȃ��Ƃ��͑ҋ@�X�e�[�g��
    if (!walk) TransitionIdleState();

    // ����ł��� ������уX�e�[�g��
    if (health <= 0) TransitionBlowState();

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
void EnemyMelee::WalkTimerUpdate(float elapsedTime)
{
    if (walkTimer > 0.0f)walkTimer -= elapsedTime;
    else turnFlag = true;
}


//����X�e�[�g�J��
void EnemyMelee::TransitionRunState()
{
    state = State::Run;
    moveSpeed = 50;
    model->PlayAnimation(static_cast<int>(state), true);

    // �^�[�Q�b�g�؂��܂�
    targetTimer = 4.0f;
}

//����X�e�[�g�X�V����
void EnemyMelee::UpdateRunState(float elapsedTime)
{
    // ����ł��� ������уX�e�[�g��
    if (health <= 0) TransitionBlowState();

    // �˒����������ː����ʂ��Ă���Ȃ�U���X�e�[�g��
    if (CheckAttackRange() && !AttackRayCheck())
    {
        TransitionAttackState();
    }

    // �ː��ʂ��ĂȂ��Ƃ�����targetTimer��
    if (AttackRayCheck())
    {
        targetTimer -= elapsedTime;
    }
    // targetTiemr���O�ɂȂ����Ƃ��^�[�Q�b�g��؂�
    if (targetTimer <= 0)
    {
        TransitionIdleState();
    }

    // �˒���������܂Ńv���C���[�Ɍ������đ��葱����
    MoveRun(direction);
}


//�U���X�e�[�g�J��
void EnemyMelee::TransitionAttackState()
{
    state = State::Attack;
    moveSpeed = 0;
    attackCooldown = 0.25f;
}

//�U���X�e�[�g�X�V����
void EnemyMelee::UpdateAttackState(float elapsedTime)
{
    // ����ł��� ������уX�e�[�g��
    if (health <= 0) {
        isAttack = false;
        TransitionBlowState();
    }

    // �~�܂�
    Move(0.0f, 0.0f, moveSpeed);
    // �U��      
    MoveAttack(1.5f);
    // �U���N�[���_�E���X�V
    AttackCooldownUpdate(elapsedTime);

    // �˒������O �������́A�ː����ʂ��Ă��Ȃ��Ȃ瑖��X�e�[�g��
    if (!CheckAttackRange() || AttackRayCheck())
    {
        isAttack = false;
        TransitionRunState();
    }
}

// �U���N�[���_�E���X�V
void EnemyMelee::AttackCooldownUpdate(float elapsedTime)
{
    if (attackCooldown > 0.0f)attackCooldown -= elapsedTime;
}


//������уX�e�[�g�J��
void EnemyMelee::TransitionBlowState()
{
    state = State::Blow;

    blowTimer = 0.3f;

    model->PlayAnimation(static_cast<int>(state), false);

    // �~�܂�
    moveSpeed = 40;

    // ������΂�
    MoveBlow();
}

//������уX�e�[�g�X�V����
void EnemyMelee::UpdateBlowState(float elapsedTime)
{
    // ������΂��^�C�}�[�X�V
    if (blowTimer > 0.0f)blowTimer -= elapsedTime;

    // ������΂����玀�S�X�e�[�g��               
    else TransitionDeathState();
}


//���S�X�e�[�g�J��
void EnemyMelee::TransitionDeathState()
{
    state = State::Death;
    model->PlayAnimation(static_cast<int>(state), false);

    // �~�܂�
    moveSpeed = 0;
    Move(0.0f, 0.0f, moveSpeed);
    velocity = { 0,0,0 };
}

//���S�X�e�[�g�X�V����
void EnemyMelee::UpdateDeathState(float elapsedTime)
{
    // ���S�A�j���[�V�����I���������ł�����
    OnDead();
    if (!model->IsPlayAnimatimon())
    {
        Destroy();
    }
}