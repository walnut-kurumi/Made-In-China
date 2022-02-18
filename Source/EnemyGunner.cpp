#include "EnemyGunner.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"
#include "Camera/CameraManager.h"

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

    TransitionIdleState();
}

void EnemyGunner::Update(float elapsedTime)
{
    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

    UpdateSpeed(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

    //オブジェクト行列更新
    UpdateTransform();
    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);
    //モデル行列更新
    model->UpdateTransform(transform);
}

void EnemyGunner::Render(ID3D11DeviceContext* dc,Shader* shader)
{
    model->Begin(dc, *shader);    
    model->Render(dc);

    // 中心座標更新
    centerPosition = position;
    centerPosition.y += 1.0f;

    //// 必要なったら追加
    debugRenderer.get()->DrawSphere(centerPosition, radius, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos2, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos3, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos4, 1.6f, Vec4(1, 0, 0, 1));
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}



// 徘徊  ←左true　false右→
void EnemyGunner::MoveWalk(bool direction)
{
    float vx;
    (direction ? vx = -1 : vx = 1);
    Move(vx, 0.0f,moveSpeed);
}

// プレイヤーを索敵
bool EnemyGunner::Search()
{
    return false;
}

// 射程距離まで走る
void EnemyGunner::MoveRun()
{
}

// 射程距離かどうか
bool EnemyGunner::CheckAttackRange()
{
    return false;
}

// 攻撃
void EnemyGunner::MoveAttack()
{
}

// 吹っ飛ぶ
void EnemyGunner::MoveBlow()
{
}



// 待機ステート遷移
void EnemyGunner::TransitionIdleState()
{
    state = State::Idle;
    model->PlayAnimation(static_cast<int>(state), true);
}

// 待機ステート更新処理
void EnemyGunner::UpdateIdleState(float elapsedTime)
{   
    // 数カウントしてから向きを変えて移動ステートへ
    //direction = true;
    
    //IdleTimerUpdate()

    // 歩きステートへ移動
    //TransitionWalkState();
}

// ターンするまでのタイマー更新
void EnemyGunner::IdleTimerUpdate()
{
    idleTimer = 0;
}


// 移動ステートへ遷移
void EnemyGunner::TransitionWalkState()
{
    state = State::Walk;
    moveSpeed = 5;
    model->PlayAnimation(static_cast<int>(state), true);
}

// 移動ステート更新処理
void EnemyGunner::UpdateWalkState(float elapsedTime)
{   
    // 徘徊
    MoveWalk(direction);
    WalkTimerUpdate();

    // プレイヤー発見したら走る
    if (Search())
    {
        TransitionRunState();
    }
}

// 止まるまでのタイマー更新処理
void EnemyGunner::WalkTimerUpdate()
{
    walkTimer = 0;
}


//走るステート遷移
void EnemyGunner::TransitionRunState()
{
    state = State::Run;
    moveSpeed = 15;
    model->PlayAnimation(static_cast<int>(state), true);
}

//走るステート更新処理
void EnemyGunner::UpdateRunState(float elapsedTime)
{    
    // 射程距離入るまでプレイヤーに向かって走り続ける
    MoveRun();
    // 射程距離内なら攻撃ステートへ
    if (CheckAttackRange())
    {
        TransitionAttackState();
    }
}


//攻撃ステート遷移
void EnemyGunner::TransitionAttackState()
{
    state = State::Attack;    
    model->PlayAnimation(static_cast<int>(state), true);
}

//攻撃ステート更新処理
void EnemyGunner::UpdateAttackState(float elapsedTime)
{
    MoveAttack();

    // 射程距離外なら走るステートへ
    if (!CheckAttackRange())
    {
        TransitionRunState();
    }
}


//吹っ飛びステート遷移
void EnemyGunner::TransitionBlowState()
{
    state = State::Blow;   
    model->PlayAnimation(static_cast<int>(state), true);
}

//吹っ飛びステート更新処理
void EnemyGunner::UpdateBlowState(float elapsedTime)
{
    // 吹っ飛ばす
    MoveBlow();

    // 吹っ飛ばしたら死亡ステートへ
    TransitionDeathState();
}


//死亡ステート遷移
void EnemyGunner::TransitionDeathState()
{
    state = State::Death;
    model->PlayAnimation(static_cast<int>(state), true);
}

//死亡ステート更新処理
void EnemyGunner::UpdateDeathState(float elapsedTime)
{
   // 死亡アニメーション終わったら消滅させる
    OnDead();
}