#include "EnemyMelee.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"
#include "Camera/CameraManager.h"

#include "HitManager.h"

// �R���X�g���N�^
EnemyMelee::EnemyMelee(ID3D11Device* device)
{

    const char* idle = "Data/Models/Enemy/MeleeAnimation/Idle.fbx";
    const char* run = "Data/Models/Enemy/MeleeAnimation/Run.fbx";
    const char* walk = "Data/Models/Enemy/MeleeAnimation/Walk.fbx";
    const char* attack = "Data/Models/Enemy/MeleeAnimation/Attack.fbx";
    const char* blow = "Data/Models/Enemy/MeleeAnimation/GetHit.fbx";
    const char* death = "Data/Models/Enemy/MeleeAnimation/Death.fbx";

     model = new Model(device, "Data/Models/Enemy/Melee.fbx",true);


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

    // ���ӂ�����
    deadEffect = new Effect("Data/Effect/enemyDead.efk");

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

// �f�X�g���N�^
EnemyMelee::~EnemyMelee()
{
    delete deadEffect;
    delete model;
    isAttack = false;
}

// ������
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

    height = 8.0f;

    centerPosition = position;
    centerPosition.y += 1.0f;
    radius = 2.5;

    isDead = false;


    isAttack = false;
    isSearch = false;

    // ���G�G���A�X�V
    UpdateSearchArea();

    TransitionWalkState();
}

// �X�V����
void EnemyMelee::Update(float elapsedTime)
{
    position.z = 0;

    if (!isDead)
    {

        (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

        // ���S���W�X�V
        UpdateCenterPosition();

        // ���˂����e�ۂƂ̏Փ˔���
        CollisionProjectileVsEnemies();
        // �U���ƃv���C���[�̔���
        if (isAttack)CollisionPanchiVsPlayer();

       
        // ���G���ԍX�V
        UpdateInvincibleTimer(elapsedTime);

        // ���G�G���A�X�V
        UpdateSearchArea();
    }
    // ���x�X�V
    UpdateSpeed(elapsedTime);


    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();
    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);
    //���f���s��X�V
    model->UpdateTransform(transform);

    // �������炵��
    FallIsDead();
}

// �`�揈��
void EnemyMelee::Render(ID3D11DeviceContext* dc, Shader* shader)
{

        model->Begin(dc, *shader);
        model->Render(dc, materialColor);

      
    if (isDead == false)
    {
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
        // ���G����Ȃ����_���[�W�^����
        if (!player->GetInvincible())
        {
            player->ApplyDamage(1, 3.0f);
            player->SetIsHit(true);
        }
        else player->SetIsHit(false);
    }
}

// �p�j  ����true�@false�E��
void EnemyMelee::MoveWalk(bool direction)
{
    // walk
    float vx;
    (direction ? vx = 1 : vx = -1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f, moveSpeed);
}

// ���G�G���A�X�V
void EnemyMelee::UpdateSearchArea()
{
    if (direction)
    {
        searchAreaPos = { position.x - 10, position.y };
        searchAreaScale = { 45, height + 5.0f };
    }
    else
    {
        searchAreaPos = { position.x - 35, position.y };
        searchAreaScale = { 45, height + 5.0f };
    }
    // �~�܂��Ă���Ƃ��͗����݂��
    if (!walk)
    {
        searchAreaPos = { position.x - 25, position.y };
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
        // �v���C���[�̒��S���W
        const Vec3& p = { player->GetCenterPosition() };
        // �G�l�~�[�̒��S���W
        const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };
        HitResult hit = {};
        // �h�A�ƃ��C�L���X�g���ē������Ă����猩���ĂȂ��̂�false
        if (DoorManager::Instance().RayCast(e, p, hit)) return false;

        if (player->GetCenterPosition().x > position.x)direction = false;
        else if (player->GetCenterPosition().x < position.x) direction = true;
        
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

    //  �ߐڍU��  
    {
        // �̂̌���
        float vx;
        (direction ? vx = -1 : vx = 1);
        angle.y = DirectX::XMConvertToRadians(90 * vx);
               
        // �U���������               
        attackPos = centerPosition;
        attackPos.x = centerPosition.x + (3.0f * vx);
        
        // �U���A�j���[�V�����Đ�
        model->PlayAnimation(static_cast<int>(state), false);
    }

    // �N�[���_�E���ݒ�
    attackCooldown = cooldown;
                  
    isAttack = false;
}

// �������
void EnemyMelee::MoveBlow()
{
    // �v���C���[�̒��S���W
    const Vec3& p = { player->GetCenterPosition() };
    // �G�l�~�[�̒��S���W
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };

    Vec3 pe = VecMath::Normalize(e - p);

    // ������΂�    100.0f = �Ռ��̋���
    AddImpulse(pe * 75.0f);

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
    idleTimer = 2.5f;
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
    if (!model->IsPlayAnimatimon() && (!CheckAttackRange() || AttackRayCheck()))
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

    if (!model->IsPlayAnimatimon()) TransitionDeathState();
}


//���S�X�e�[�g�J��
void EnemyMelee::TransitionDeathState()
{
    state = State::Death;
    model->PlayAnimation(static_cast<int>(state), false);
    handle = deadEffect->Play(centerPosition, 1.0f);
    // �~�܂�
    moveSpeed = 0;
    Move(0.0f, 0.0f, moveSpeed);
    velocity = { 0,0,0 };

    OnDead();
}

//���S�X�e�[�g�X�V����
void EnemyMelee::UpdateDeathState(float elapsedTime)
{
    // ���S�A�j���[�V�����I���������ł�����
    if (!model->IsPlayAnimatimon())
    {                
        //Destroy();
    }
}