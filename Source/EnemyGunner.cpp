#include "EnemyGunner.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"
#include "Camera/CameraManager.h"
#include "EnemyBulletStraight.h"

#include "HitManager.h"

// �R���X�g���N�^
EnemyGunner::EnemyGunner(ID3D11Device* device)
{


    const char* idle = "Data/Models/Enemy/GunnerAnimation/Idle.fbx";
    const char* run = "Data/Models/Enemy/GunnerAnimation/Run.fbx";
    const char* walk = "Data/Models/Enemy/GunnerAnimation/Walk.fbx";
    const char* attack = "Data/Models/Enemy/GunnerAnimation/Attack.fbx";
    const char* blow = "Data/Models/Enemy/GunnerAnimation/GetHit.fbx";
    const char* death = "Data/Models/Enemy/GunnerAnimation/Death.fbx";

    model = new Model(device, "Data/Models/Enemy/Gunner.fbx", true);


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
      
    // �G�t�F�N�g
    deadEffect = new Effect("Data/Effect/enemyDead.efk");

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

// �f�X�g���N�^
EnemyGunner::~EnemyGunner()
{
    delete deadEffect;
    delete model;    
    isAttack = false;
}

// ������
void EnemyGunner::Init()
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
void EnemyGunner::Update(float elapsedTime)
{
    position.z = 0;

    if (!isDead)
    {

        (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

        // ���S���W�X�V
        UpdateCenterPosition();

        // ���˂����e�ۂƂ̏Փ˔���
        CollisionProjectileVsEnemies();
        // ���˂����e�ۂƃv���C���[�̏Փ˔���
        CollisionProjectileVsPlayer();


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

    FallIsDead();
}

// �`�揈��
void EnemyGunner::Render(ID3D11DeviceContext* dc,Shader* shader)
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
void EnemyGunner::CollisionProjectileVsEnemies()
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

// �v���C���[�ƒe�̏Փ˔���
void EnemyGunner::CollisionProjectileVsPlayer()
{
    EnemyBulletManager& enemyBManager = EnemyBulletManager::Instance();
    int enemyBCount = enemyBManager.GetProjectileCount();
    for (int i = 0; i < enemyBCount; ++i)
    {
        EnemyBullet* enemyB = enemyBManager.GetProjectile(i);
        // �Փˏ���
        if (Collision::SphereVsSphere(enemyB->GetPosition(), player->GetCenterPosition(), enemyB->GetRadius(), player->GetRadius()))
        {
            // ���˂��ĂȂ�������
            if (!enemyB->GetReflectionFlag())
            {
                // ���G����Ȃ����_���[�W�^����
                if (!player->GetInvincible())
                {
                    player->ApplyDamage(1, 0.8f);
                    player->SetIsHit(true);
                }
                else player->SetIsHit(false);
            }
        }
    }
}

// �p�j  ����true�@false�E��
void EnemyGunner::MoveWalk(bool direction)
{
    // walk
    float vx;
    (direction ? vx = 1 : vx = -1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f,moveSpeed);
}

// ���G�G���A�X�V
void EnemyGunner::UpdateSearchArea()
{    
    if (this->direction)
    {        
        searchAreaPos = { position.x - 10, position.y };        
        searchAreaScale = { 50, height + 5.0f };
    }
    else
    {        
        searchAreaPos = { position.x - 35, position.y };        
        searchAreaScale = { 50, height + 5.0f };
    }    
    // �~�܂��Ă���Ƃ��͗����݂��
    if (!walk)
    {
        searchAreaPos = { position.x - 30, position.y };
        searchAreaScale = { 60, height + 5.0f };
    }
}

// ���S���W
void EnemyGunner::UpdateCenterPosition()
{
    // ���S���W�X�V
    centerPosition = position;
    centerPosition.y += height/2.0f;
}

// �v���C���[�����G
bool EnemyGunner::Search()
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

        if (player->GetCenterPosition().x > position.x)this->direction = false;
        else if (player->GetCenterPosition().x < position.x) this->direction = true;
      
        isSearch = true;

        return true;
    }
    //�������Ă��Ȃ����false    
    return false;
}

// �˒������܂ő���
void EnemyGunner::MoveRun(bool direction)
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
bool EnemyGunner::CheckAttackRange()
{
    // �v���C���[�̍��W�ƃK���i�[�̍��W�Ńx�N�g���쐬
    Vec3 distance = { player->GetCenterPosition() - position };
    //�@�x�N�g�����狗���擾    
    float range =  VecMath::LengthVec3(distance);
    // �˒�������菬�����Ȃ�true
    if (attackRange >= range)return true;
    // �˒��������傫���Ȃ�false
    else return false;    
}

// �U��
void EnemyGunner::MoveAttack(float cooldown)
{   
    if (attackCooldown > 0.0f) return;
    // �U���ӂ炮
    isAttack = true;       


    if (player->GetCenterPosition().x > position.x)this->direction = false;
    else if (player->GetCenterPosition().x < position.x) this->direction = true;
    // �̂̌���
    float vx;
    (this->direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);


    ID3D11Device* device = Graphics::Ins().GetDevice();          

    // ���i�e�۔���   
    {
        // �U���A�j���[�V�����Đ�
        model->PlayAnimation(static_cast<int>(state), false);

        // �v���C���[�̒��S���W
        const Vec3& p = { player->GetCenterPosition() };
        // �G�l�~�[�̒��S���W
        const Vec3& e = { centerPosition.x,centerPosition.y + 2.0f,0.0f };

        // ���˂������       
        // �v���C���[�Ɍ�������
        Vec3 pe = { p - e };
        pe =  VecMath::Normalize(pe);

        SEGun = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Enemybullet���1.wav", false);
        //SEGun = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Enemybullet���2.wav", false);
        SEGun.get()->Play(1.0f);

        // ����
        EnemyBulletStraight* bullet = new EnemyBulletStraight(device, &EnemyBulletManager::Instance());
        bullet->Launch(pe, e);                    
    }

    // �N�[���_�E���ݒ�
    attackCooldown = cooldown;
}

// �������
void EnemyGunner::MoveBlow()
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
bool EnemyGunner::AttackRayCheck()
{
    // �v���C���[�̒��S���W
    const Vec3& p = { player->GetCenterPosition() };
    // �G�l�~�[�̒��S���W
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };
  

    // �v���C���[�����փ��C�L���X�g
    HitResult hit;

    if (StageManager::Instance().RayCast(e, p, hit))
    {
        if (hit.penetrate) return false;

        return true;
    }

    return false;
}



// �ҋ@�X�e�[�g�J��
void EnemyGunner::TransitionIdleState()
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
void EnemyGunner::UpdateIdleState(float elapsedTime)
{   
    // ����ł��� ������уX�e�[�g��
    if (health <= 0) TransitionBlowState();

    Move(0.0f, 0.0f, moveSpeed);
    IdleTimerUpdate(elapsedTime);

    if (walkFlag)
    {
        // ���J�E���g���Ă��������ς��Ĉړ��X�e�[�g��
        this->direction = !this->direction;
        // �����X�e�[�g�ֈړ�
        TransitionWalkState();
    }

    // �v���C���[���������瑖��
    if (Search())
    {
        // �˒����������ː����ʂ��Ă���Ȃ�U���X�e�[�g��
        if (CheckAttackRange() && !AttackRayCheck())
        {
            TransitionAttackState();
        }

        TransitionRunState();
    }
}

// �^�[������܂ł̃^�C�}�[�X�V
void EnemyGunner::IdleTimerUpdate(float elapsedTime)
{
    if (idleTimer > 0.0f)idleTimer-= elapsedTime;    
    else walkFlag = true;
}


// �ړ��X�e�[�g�֑J��
void EnemyGunner::TransitionWalkState()
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
void EnemyGunner::UpdateWalkState(float elapsedTime)
{   
    // �������Ȃ��Ƃ��͑ҋ@�X�e�[�g��
    if (!walk) TransitionIdleState();

    // ����ł��� ������уX�e�[�g��
    if (health <= 0) TransitionBlowState();

    // �p�j
    if (walkFlag)
    {
        MoveWalk(this->direction);
        WalkTimerUpdate(elapsedTime);
    }

    if (turnFlag)
    {
        TransitionIdleState();        
    }


    // �v���C���[���������瑖��
    if (Search())
    {
        // �˒����������ː����ʂ��Ă���Ȃ�U���X�e�[�g��
        if (CheckAttackRange() && !AttackRayCheck())
        {
            TransitionAttackState();
        }

        TransitionRunState();
    }
}

// �~�܂�܂ł̃^�C�}�[�X�V����
void EnemyGunner::WalkTimerUpdate(float elapsedTime)
{
    if (walkTimer > 0.0f)walkTimer-= elapsedTime;
    else turnFlag = true;
}


//����X�e�[�g�J��
void EnemyGunner::TransitionRunState()
{
    state = State::Run;
    moveSpeed = 45;
    model->PlayAnimation(static_cast<int>(state), true);

    // �^�[�Q�b�g�؂��܂�
    targetTimer = 2.0f;
}

//����X�e�[�g�X�V����
void EnemyGunner::UpdateRunState(float elapsedTime)
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
    MoveRun(this->direction);
}


//�U���X�e�[�g�J��
void EnemyGunner::TransitionAttackState()
{
    state = State::Attack;
    moveSpeed = 0;
    attackCooldown = 0.3f;        
}

//�U���X�e�[�g�X�V����
void EnemyGunner::UpdateAttackState(float elapsedTime)
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
void EnemyGunner::AttackCooldownUpdate(float elapsedTime)
{
    if (attackCooldown > 0.0f)attackCooldown -= elapsedTime;
}


//������уX�e�[�g�J��
void EnemyGunner::TransitionBlowState()
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
void EnemyGunner::UpdateBlowState(float elapsedTime)
{  
    // ������΂��^�C�}�[�X�V
    if (blowTimer > 0.0f)blowTimer -= elapsedTime;    

    // ������΂����玀�S�X�e�[�g��               
    else TransitionDeathState();    

    if(!model->IsPlayAnimatimon()) TransitionDeathState();    
}


//���S�X�e�[�g�J��
void EnemyGunner::TransitionDeathState()
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
void EnemyGunner::UpdateDeathState(float elapsedTime)
{    
   // ���S�A�j���[�V�����I���������ł�����
    if (!model->IsPlayAnimatimon())
    {                
        //Destroy();
    }
}