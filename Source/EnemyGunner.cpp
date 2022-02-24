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
    health = 1;

    centerPosition = position;
    centerPosition.y += 1.0f;
    radius = 2.5;

    isDead = false;

    direction = true;

    TransitionWalkState();
}

void EnemyGunner::Update(float elapsedTime)
{

    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

   const Vec3& im = { -5,5,0 };
    
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_X) {
        health = 0;
        AddImpulse(im);
    }
   
    // 速度更新
    UpdateSpeed(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

    // 弾丸更新処理
    bulletManager.Update(elapsedTime);

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

    // 弾丸描画処理
    bulletManager.Render(dc, shader);

    // 中心座標更新
    centerPosition = position;
    centerPosition.y += height;
   
    //// 必要なったら追加
    debugRenderer.get()->DrawSphere(centerPosition, radius, Vec4(1, 0, 0, 1));
    debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x, searchAreaPos.y,6.0), 1.0f, Vec4(0, 1, 0, 1));
    debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x + searchAreaScale.x, searchAreaPos.y + searchAreaScale.y,6.0), 1.0f, Vec4(0, 1, 0, 1));
    //debugRenderer.get()->DrawSphere(copos3, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos4, 1.6f, Vec4(1, 0, 0, 1));
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}



// 徘徊  ←左true　false右→
void EnemyGunner::MoveWalk(bool direction)
{
    // walk
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f,moveSpeed);

    // 索敵範囲更新
    searchAreaPos = { position.x - (10 * vx), position.y -2.0f };
    searchAreaScale = { 35 * vx, height + 2.0f };
}

// プレイヤーを索敵
bool EnemyGunner::Search()
{
    // TODO:サーチ範囲見直す　後ろ怪しい
    //searchArea（短形） と playerPos（点）で当たり判定
    
    //当たっていたら索敵範囲内なのでtrue
    if (Collision::PointVsRect(playerPos, searchAreaPos, searchAreaScale))
    {
        if (playerPos.x > position.x)direction = false;
        else if (playerPos.x < position.x) direction = true;
        return true;
    }
    //当たっていなければfalse    
    else return false;    
}

// 射程距離まで走る
void EnemyGunner::MoveRun(bool direction)
{
    // 向きをプレイヤーの方向へ
    if (playerPos.x > position.x)direction = false;
    else if (playerPos.x < position.x) direction = true;

    // run
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f, moveSpeed);

    // 索敵範囲更新
    searchAreaPos = { position.x - (10 * vx), position.y - 2.0f };
    searchAreaScale = { 35 * vx, height + 2.0f };
}

// 射程距離かどうか
bool EnemyGunner::CheckAttackRange()
{
    // プレイヤーの座標とガンナーの座標でベクトル作成
    Vec3 distance = { Vec3(playerPos.x,playerPos.y,0.0f) - position };
    //　ベクトルから距離取得    
    float range =  VecMath::LengthVec3(distance);
    // 射程距離より小さいならtrue
    if (attackRange >= range)return true;
    // 射程距離より大きいならfalse
    else return false;    
}

// 攻撃
void EnemyGunner::MoveAttack(float cooldown)
{
    if (attackCooldown > 0.0f) return;

    // 攻撃する向きをプレイヤーの方向へ
    if (playerPos.x > position.x)direction = false;
    else if (playerPos.x < position.x) direction = true;

    // クールダウン設定
    attackCooldown = cooldown;

    ID3D11Device* device = Graphics::Ins().GetDevice();
    
    // TODO:プレイヤーの中心座標に向けて発射
    // 直進弾丸発射   
    {
        float vx;
        (direction ? vx = -1 : vx = 1);
        angle.y = DirectX::XMConvertToRadians(90 * vx);

        // 発射する向き
        DirectX::XMFLOAT3 dir;
        dir.x = vx;
        dir.y = 0.0f;
        dir.z = 0.0f;       
        // 発射
        EnemyBulletStraight* bullet = new EnemyBulletStraight(device, &bulletManager);
        bullet->Launch(dir, centerPosition);        
    }
}

// 吹っ飛ぶ
void EnemyGunner::MoveBlow()
{    
    // プレイヤーの攻撃方向へ吹き飛ばす
    const float blowPower = 10.0f;
    Vec3 blowDirection = {};
 
    // プレイヤーの中心座標
    const Vec3& p = { playerPos.x,playerPos.y,0.0f };
    // エネミーの中心座標
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };


    float vx = e.x - p.x;
    float vy = e.y - p.y;
    float lengthXY = sqrtf(vx * vx + vy * vy);
    vx /= lengthXY;
    vy /= lengthXY;
   
    blowDirection.x = vx * blowPower;
    blowDirection.y = vy * blowPower;
    blowDirection.z = 0.0f;

    AddImpulse(blowDirection);
 
}



// 待機ステート遷移
void EnemyGunner::TransitionIdleState()
{
    state = State::Idle;
    turnFlag = false;
    walkFlag = false;
    idleTimer = 5.0f;
    moveSpeed = 0;
    model->PlayAnimation(static_cast<int>(state), true);
}

// 待機ステート更新処理
void EnemyGunner::UpdateIdleState(float elapsedTime)
{   
    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) TransitionBlowState();

    Move(0.0f, 0.0f, moveSpeed);
    IdleTimerUpdate(elapsedTime);

    if (walkFlag)
    {
        // 数カウントしてから向きを変えて移動ステートへ
        direction = !direction;
        // 歩きステートへ移動
        TransitionWalkState();
    }
}

// ターンするまでのタイマー更新
void EnemyGunner::IdleTimerUpdate(float elapsedTime)
{
    if (idleTimer > 0.0f)idleTimer-= elapsedTime;    
    else walkFlag = true;
}


// 移動ステートへ遷移
void EnemyGunner::TransitionWalkState()
{
    state = State::Walk;
    turnFlag = false;
    walkFlag = true;
    walkTimer = 5.0f;
    moveSpeed = 5;
    model->PlayAnimation(static_cast<int>(state), true);
}

// 移動ステート更新処理
void EnemyGunner::UpdateWalkState(float elapsedTime)
{   
    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) TransitionBlowState();

    // 徘徊
    if (walkFlag)
    {
        MoveWalk(direction);
        WalkTimerUpdate(elapsedTime);
    }

    if (turnFlag)
    {
        TransitionIdleState();        
    }


    // プレイヤー発見したら走る
    if (Search())
    {
        TransitionRunState();
    }
}

// 止まるまでのタイマー更新処理
void EnemyGunner::WalkTimerUpdate(float elapsedTime)
{
    if (walkTimer > 0.0f)walkTimer-= elapsedTime;
    else turnFlag = true;
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
    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) TransitionBlowState();

    // 射程距離内なら攻撃ステートへ
    if (CheckAttackRange())
    {
        TransitionAttackState();
    }
    // 射程距離入るまでプレイヤーに向かって走り続ける
    MoveRun(direction);
}


//攻撃ステート遷移
void EnemyGunner::TransitionAttackState()
{
    state = State::Attack; 
    moveSpeed = 0;
    attackCooldown = 0.75f;
    model->PlayAnimation(static_cast<int>(state), true);
}

//攻撃ステート更新処理
void EnemyGunner::UpdateAttackState(float elapsedTime)
{
    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) TransitionBlowState();

    // 止まる
    Move(0.0f, 0.0f, moveSpeed);
    // 攻撃      
    MoveAttack(1.5f);    
    // 攻撃クールダウン更新
    AttackCooldownUpdate(elapsedTime);

    // 射程距離外なら走るステートへ
    if (!CheckAttackRange())
    {
        TransitionRunState();
    }
}

// 攻撃クールダウン更新
void EnemyGunner::AttackCooldownUpdate(float elapsedTime)
{
    if (attackCooldown > 0.0f)attackCooldown -= elapsedTime;
}


//吹っ飛びステート遷移
void EnemyGunner::TransitionBlowState()
{
    state = State::Blow;  

    blowTimer = 0.4f;

    model->PlayAnimation(static_cast<int>(state), true);

    // 止まる
    moveSpeed = 0;
    Move(0.0f, 0.0f, moveSpeed);
}

//吹っ飛びステート更新処理
void EnemyGunner::UpdateBlowState(float elapsedTime)
{
    // TODO:吹っ飛ばない不具合を直す
    // 吹っ飛ばす
    MoveBlow();
    
    // 吹き飛ばしタイマー更新
    if (blowTimer > 0.0f)blowTimer -= elapsedTime;    

    // 吹っ飛ばしたら死亡ステートへ
    if(blowTimer < 0.0f)TransitionDeathState();
}


//死亡ステート遷移
void EnemyGunner::TransitionDeathState()
{
    state = State::Death;    
    model->PlayAnimation(static_cast<int>(state), true);

    // 止まる
    moveSpeed = 0;
    Move(0.0f, 0.0f, moveSpeed);
}

//死亡ステート更新処理
void EnemyGunner::UpdateDeathState(float elapsedTime)
{    
   // 死亡アニメーション終わったら消滅させる
    OnDead();
}