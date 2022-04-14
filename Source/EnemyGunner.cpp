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

    model = new Model(device, "Data/Models/Player/Jummo.fbx");

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

    materialColor = { 0,0,0,0 };

    centerPosition = position;
    centerPosition.y += 1.0f;
    radius = 2.5;

    isDead = false;

    direction = false;

    TransitionWalkState();
}

void EnemyGunner::Update(float elapsedTime)
{
    if (isDead)return;

    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);
 
    // 中心座標更新
    UpdateCenterPosition();

    // 反射した弾丸との衝突判定
    CollisionProjectileVsEnemies();

    // 速度更新
    UpdateSpeed(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

    // 索敵エリア更新
    UpdateSearchArea();

    //オブジェクト行列更新
    UpdateTransform();
    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);
    //モデル行列更新
    model->UpdateTransform(transform);
}

void EnemyGunner::Render(ID3D11DeviceContext* dc,Shader* shader)
{
    if (isDead == false)
    {
        model->Begin(dc, *shader);
        model->Render(dc, materialColor);

        // 弾丸描画処理
        EnemyBulletManager::Instance().Render(dc, shader);

        //// 必要なったら追加
        debugRenderer.get()->DrawSphere(centerPosition, radius, Vec4(1, 0, 0, 1));
        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x, searchAreaPos.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));
        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x + searchAreaScale.x, searchAreaPos.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));
        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x, searchAreaPos.y + searchAreaScale.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));
        debugRenderer.get()->DrawSphere(Vec3(searchAreaPos.x + searchAreaScale.x, searchAreaPos.y + searchAreaScale.y, 6.0), 1.0f, Vec4(0, 0.5f, 1, 1));

        debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
    }
}


// 反射した弾丸との衝突判定
void EnemyGunner::CollisionProjectileVsEnemies()
{
    EnemyBulletManager& enemyBManager = EnemyBulletManager::Instance();
    int enemyBCount = enemyBManager.GetProjectileCount();
    for (int i = 0; i < enemyBCount; ++i)
    {
        EnemyBullet* enemyB = enemyBManager.GetProjectile(i);
        // 衝突処理
        if (Collision::SphereVsSphere(enemyB->GetPosition(), centerPosition, enemyB->GetRadius(), radius))
        {
            // 反射してたら
            if (enemyB->GetReflectionFlag())
            {
                // ダメージ与える
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

// プレイヤーとの衝突判定
void EnemyGunner::CollisionProjectileVsPlayer()
{
    EnemyBulletManager& enemyBManager = EnemyBulletManager::Instance();
    int enemyBCount = enemyBManager.GetProjectileCount();
    for (int i = 0; i < enemyBCount; ++i)
    {
        EnemyBullet* enemyB = enemyBManager.GetProjectile(i);
        // 衝突処理
        if (Collision::SphereVsSphere(enemyB->GetPosition(), player->GetCenterPosition(), enemyB->GetRadius(), player->GetRadius()))
        {
            // 反射してなかったら
            if (!enemyB->GetReflectionFlag())
            {
                // ダメージ与える
                player->ApplyDamage(1, 0.8f);                
            }
        }
    }
}

// 徘徊  ←左true　false右→
void EnemyGunner::MoveWalk(bool direction)
{
    // walk
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f,moveSpeed);
}

// 索敵エリア更新
void EnemyGunner::UpdateSearchArea()
{    
    if (!direction)
    {        
        searchAreaPos = { position.x - 10, position.y - 2.0f };        
        searchAreaScale = { 45, height + 2.0f };
    }
    else
    {        
        searchAreaPos = { position.x - 35, position.y - 2.0f };        
        searchAreaScale = { 45, height + 2.0f };
    }    
}

// 中心座標
void EnemyGunner::UpdateCenterPosition()
{
    // 中心座標更新
    centerPosition = position;
    centerPosition.y += height;
}

// プレイヤーを索敵
bool EnemyGunner::Search()
{      
    //searchArea（短形） と playerPos（点）で当たり判定    
    //当たっていたら索敵範囲内なのでtrue
    if (Collision::PointVsRect(Vec2(player->GetCenterPosition().x, player->GetCenterPosition().y), searchAreaPos, searchAreaScale))
    {
        if (player->GetCenterPosition().x > position.x)direction = false;
        else if (player->GetCenterPosition().x < position.x) direction = true;

        materialColor = { 1.0f,0.25f,0.25f,1.0f };

        return true;
    }
    //当たっていなければfalse    
    return false;
}

// 射程距離まで走る
void EnemyGunner::MoveRun(bool direction)
{
    // 向きをプレイヤーの方向へ
    if (player->GetCenterPosition().x > position.x)direction = false;
    else if (player->GetCenterPosition().x < position.x) direction = true;

    // run
    float vx;
    (direction ? vx = -1 : vx = 1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f, moveSpeed);
}

// 射程距離かどうか
bool EnemyGunner::CheckAttackRange()
{
    // プレイヤーの座標とガンナーの座標でベクトル作成
    Vec3 distance = { player->GetCenterPosition() - position };
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
    if (attackCooldown > 0.0f)
    {       
        return;
    }

    // 攻撃する向きをプレイヤーの方向へ
    if (player->GetCenterPosition().x > position.x)direction = false;
    else if (player->GetCenterPosition().x < position.x) direction = true;

    // クールダウン設定
    attackCooldown = cooldown;

    ID3D11Device* device = Graphics::Ins().GetDevice();
        
    // 直進弾丸発射   
    {
        // 体の向き
        float vx;
        (direction ? vx = -1 : vx = 1);
        angle.y = DirectX::XMConvertToRadians(90 * vx);

        // 攻撃アニメーション再生
        model->PlayAnimation(static_cast<int>(state), false);

        // プレイヤーの中心座標
        const Vec3& p = { player->GetCenterPosition() };
        // エネミーの中心座標
        const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };

        // 発射する向き       
        // プレイヤーに向かって
        Vec3 pe = { p - e };
        VecMath::Normalize(pe);
        pe *= 0.1f;

        // 発射
        EnemyBulletStraight* bullet = new EnemyBulletStraight(device, &EnemyBulletManager::Instance());
        bullet->Launch(pe, e);        
    }
}

// 吹っ飛ぶ
void EnemyGunner::MoveBlow()
{                
    // プレイヤーの中心座標
    const Vec3& p = { player->GetCenterPosition() };
    // エネミーの中心座標
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };
        
    float vx = e.x - p.x;
    float vy = e.y - p.y;
    float lengthXY = sqrtf(vx * vx + vy * vy);
    vx /= lengthXY;
    vy /= lengthXY;
          
    // 吹っ飛ばす    
    AttackMove(vx, vy, moveSpeed);
 
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

    // プレイヤー発見したら走る
    if (Search())
    {
        TransitionRunState();
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
    // 歩き回らないときは待機ステートへ
    if (!walk) TransitionIdleState();

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
    moveSpeed = 50;
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

    blowTimer = 0.3f;

    model->PlayAnimation(static_cast<int>(state), false);

    // 止まる
    moveSpeed = 40;    
}

//吹っ飛びステート更新処理
void EnemyGunner::UpdateBlowState(float elapsedTime)
{
    // 吹っ飛ばす
    MoveBlow();
    
    // 吹き飛ばしタイマー更新
    if (blowTimer > 0.0f)blowTimer -= elapsedTime;    

    // 吹っ飛ばしたら死亡ステートへ               
    else TransitionDeathState();    
}


//死亡ステート遷移
void EnemyGunner::TransitionDeathState()
{
    state = State::Death;    
    model->PlayAnimation(static_cast<int>(state), false);

    // 止まる
    moveSpeed = 0;
    Move(0.0f, 0.0f, moveSpeed);
    velocity = { 0,0,0 };
}

//死亡ステート更新処理
void EnemyGunner::UpdateDeathState(float elapsedTime)
{    
   // 死亡アニメーション終わったら消滅させる
    OnDead();
    if (!model->IsPlayAnimatimon())
    {
        Destroy();
    }
}