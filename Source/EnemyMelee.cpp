#include "EnemyMelee.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"
#include "Camera/CameraManager.h"

#include "HitManager.h"

// コンストラクタ
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
    TransitionIdleState();

    // えふぇくと
    deadEffect = new Effect("Data/Effect/enemyDead.efk");

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

// デストラクタ
EnemyMelee::~EnemyMelee()
{
    delete deadEffect;
    delete model;
    isAttack = false;
}

// 初期化
void EnemyMelee::Init()
{
    penetrate = false;

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

    atk = false;

    // 索敵エリア更新
    UpdateSearchArea();

    TransitionWalkState();
}

// 更新処理
void EnemyMelee::Update(float elapsedTime)
{
    position.z = 0;

    if (!isDead)
    {

        (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

        // 中心座標更新
        UpdateCenterPosition();

        // 反射した弾丸との衝突判定
        CollisionProjectileVsEnemies();
        // 攻撃とプレイヤーの判定
        if (atk)CollisionPanchiVsPlayer();

       
        // 無敵時間更新
        UpdateInvincibleTimer(elapsedTime);

        // 索敵エリア更新
        UpdateSearchArea();      
    }
    // 速度更新
    UpdateSpeed(elapsedTime);


    //オブジェクト行列更新
    UpdateTransform();
    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);
    //モデル行列更新
    model->UpdateTransform(transform);

    // おちたらしぬ
    FallIsDead();
}

// 描画処理
void EnemyMelee::Render(ID3D11DeviceContext* dc, Shader* shader, bool slow)
{   
    if (slow) {
        model->Begin(dc, Shaders::Ins()->GetOutline());
        RenderStateKARI::SetCullMode(RenderStateKARI::CU_BACK);
        model->Render(dc);
    }
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
        if(atk)debugRenderer.get()->DrawSphere(attackPos, attackRadius, Vec4(1.0f, 0, 0.5f, 1));

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


// 反射した弾丸との衝突判定
void EnemyMelee::CollisionProjectileVsEnemies()
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

// 攻撃とプレイヤーの判定
void EnemyMelee::CollisionPanchiVsPlayer()
{
    // 衝突処理
    if (Collision::SphereVsSphere(attackPos, player->GetCenterPosition(), attackRadius, player->GetRadius())) {        
        // 無敵じゃない時ダメージ与える
        if (!player->GetInvincible())
        {
            player->ApplyDamage(1, 3.0f);
            player->SetIsHit(true);
        }
        else player->SetIsHit(false);
    }
}

// 徘徊  ←左true　false右→
void EnemyMelee::MoveWalk(bool direction)
{
    // walk
    float vx;
    (direction ? vx = 1 : vx = -1);
    angle.y = DirectX::XMConvertToRadians(90 * vx);
    Move(vx, 0.0f, moveSpeed);
}

// 索敵エリア更新
void EnemyMelee::UpdateSearchArea()
{
    if (direction)
    {
        searchAreaPos = { position.x - 10, position.y };
        searchAreaScale = { 50, height + 5.0f };
    }
    else
    {
        searchAreaPos = { position.x - 35, position.y };
        searchAreaScale = { 50, height + 5.0f };
    }
    // 止まっているときは両方みれる
    if (!walk)
    {
        searchAreaPos = { position.x - 30, position.y };
        searchAreaScale = { 60, height + 5.0f };
    }
}

// 中心座標
void EnemyMelee::UpdateCenterPosition()
{
    // 中心座標更新
    centerPosition = position;
    centerPosition.y += height / 2.0f;
}

// プレイヤーを索敵
bool EnemyMelee::Search()
{
    // isSearch = true ならreturn true
    if (isSearch)return true;

    //searchArea（短形） と playerPos（点）で当たり判定    
    //当たっていたら索敵範囲内なのでtrue
    if (Collision::PointVsRect(Vec2(player->GetCenterPosition().x, player->GetCenterPosition().y), searchAreaPos, searchAreaScale))
    {
        // プレイヤーの中心座標
        const Vec3& p = { player->GetCenterPosition() };
        // エネミーの中心座標
        const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };
        HitResult hit = {};
        // ドアとレイキャストして当たっていたら見つけてないのでfalse
        if (DoorManager::Instance().RayCast(e, p, hit)) return false;
        if (StageManager::Instance().RayCast(e, p, hit)) return false;

        if (player->GetCenterPosition().x > position.x)this->direction = false;
        else if (player->GetCenterPosition().x < position.x) this->direction = true;
        
        isSearch = true;

        return true;
    }
    //当たっていなければfalse    
    return false;
}

// 射程距離まで走る
void EnemyMelee::MoveRun(bool direction)
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
bool EnemyMelee::CheckAttackRange()
{
    // プレイヤーの座標とガンナーの座標でベクトル作成
    Vec3 distance = { player->GetCenterPosition() - position };
    //　ベクトルから距離取得    
    float range = VecMath::LengthVec3(distance);
    // 射程距離より小さいならtrue
    if (attackRange >= range)return true;
    // 射程距離より大きいならfalse
    else return false;
}

// 攻撃
void EnemyMelee::MoveAttack(float cooldown)
{
    if (attackCooldown > 0.0f) return;    
    // 攻撃ふらぐ
    isAttack = true;   

    //  近接攻撃  
    {                
        // 止まる
        Move(0.0f, 0.0f, moveSpeed);

        // 体の向き
        float vx;
        (this->direction ? vx = -1 : vx = 1);
        angle.y = DirectX::XMConvertToRadians(90 * vx);
        // 攻撃する向き               
        /*attackPos = centerPosition;
        attackPos.x = centerPosition.x + (3.0f * vx);*/

        // 手の位置
        SkinnedMesh::Animation::Keyframe::Node * rightHand = model->FindNode("joint19");
        DirectX::XMFLOAT4X4 p = rightHand->globalTransform;
        DirectX::XMFLOAT4X4 p2;
        using namespace DirectX;
        XMStoreFloat4x4(&p2, XMLoadFloat4x4(&rightHand->globalTransform) * XMLoadFloat4x4(&transform));
        attackPos = { p2._41, p2._42, p2._43 };


        SEEPunch = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\EnemyPunch.wav", false);
        SEEPunch.get()->Play(0.8f);

        // 攻撃アニメーション再生
        model->PlayAnimation(static_cast<int>(state), false);
    }

    // クールダウン設定
    attackCooldown = cooldown;
}

// 吹っ飛ぶ
void EnemyMelee::MoveBlow()
{
    // プレイヤーの中心座標
    const Vec3& p = { player->GetCenterPosition() };
    // エネミーの中心座標
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };

    Vec3 pe = VecMath::Normalize(e - p);

    // 吹っ飛ばす    100.0f = 衝撃の強さ
    AddImpulse(pe * 75.0f);

}

//射線が通っていないか
bool EnemyMelee::AttackRayCheck()
{
    // プレイヤーの中心座標
    const Vec3& p = { player->GetCenterPosition() };
    // エネミーの中心座標
    const Vec3& e = { centerPosition.x,centerPosition.y,0.0f };


    // プレイヤー方向へレイキャスト
    HitResult hit;

    if (StageManager::Instance().RayCast(e, p, hit))
    {
        if (hit.penetrate)
        {
            penetrate = true;
            return false;
        }
        else penetrate = false;
        
        return true;
    }

    return false;
}



// 待機ステート遷移
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

// 待機ステート更新処理
void EnemyMelee::UpdateIdleState(float elapsedTime)
{
    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) TransitionBlowState();

    Move(0.0f, 0.0f, moveSpeed);
    IdleTimerUpdate(elapsedTime);

    if (walkFlag)
    {
        // 数カウントしてから向きを変えて移動ステートへ
        this->direction = !this->direction;
        // 歩きステートへ移動
        TransitionWalkState();
    }

    // プレイヤー発見したら走る
    if (Search())
    {
        // 射程距離内かつ射線が通っているなら攻撃ステートへ
        if (CheckAttackRange() && !AttackRayCheck())
        {
            TransitionAttackState();
        }

        TransitionRunState();
    }
}

// ターンするまでのタイマー更新
void EnemyMelee::IdleTimerUpdate(float elapsedTime)
{
    if (idleTimer > 0.0f)idleTimer -= elapsedTime;
    else walkFlag = true;
}


// 移動ステートへ遷移
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

// 移動ステート更新処理
void EnemyMelee::UpdateWalkState(float elapsedTime)
{
    // 歩き回らないときは待機ステートへ
    if (!walk) TransitionIdleState();

    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) TransitionBlowState();
    if (isDead) TransitionBlowState();

    // 徘徊
    if (walkFlag)
    {
        MoveWalk(this->direction);
        WalkTimerUpdate(elapsedTime);
    }

    if (turnFlag)
    {
        TransitionIdleState();
    }


    // プレイヤー発見したら走る
    if (Search())
    {
        // 射程距離内かつ射線が通っているなら攻撃ステートへ
        if (CheckAttackRange() && !AttackRayCheck())
        {
            TransitionAttackState();
        }

        TransitionRunState();
    }
}

// 止まるまでのタイマー更新処理
void EnemyMelee::WalkTimerUpdate(float elapsedTime)
{
    if (walkTimer > 0.0f)walkTimer -= elapsedTime;
    else turnFlag = true;
}


//走るステート遷移
void EnemyMelee::TransitionRunState()
{
    state = State::Run;
    moveSpeed = 60;
    model->PlayAnimation(static_cast<int>(state), true);

    // ターゲット切れるまで
    targetTimer = 2.0f;
}

//走るステート更新処理
void EnemyMelee::UpdateRunState(float elapsedTime)
{
    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) TransitionBlowState();

    // 射程距離内かつ射線が通っているなら攻撃ステートへ
    if (CheckAttackRange() && !AttackRayCheck())
    {
        TransitionAttackState();
    }

    // 射線通ってないときだけtargetTimer回す
    if (AttackRayCheck())
    {
        targetTimer -= elapsedTime;
    }
    // targetTiemrが０になったときターゲットを切る
    if (targetTimer <= 0)
    {
        TransitionIdleState();
    }

    // 射程距離入るまでプレイヤーに向かって走り続ける
    MoveRun(this->direction);
}


//攻撃ステート遷移
void EnemyMelee::TransitionAttackState()
{
    state = State::Attack;
    moveSpeed = 0;
    attackCooldown = 0.1f;
    model->PlayAnimation(static_cast<int>(State::Walk), true);
}

//攻撃ステート更新処理
void EnemyMelee::UpdateAttackState(float elapsedTime)
{
    // 死んでたら 吹っ飛びステートへ
    if (health <= 0) {
        isAttack = false;
        atk = false;
        TransitionBlowState();
    }

    // 任意のアニメーション再生区間でのみ衝突判定処理をする
    float animationTime = model->GetCurrentAnimationSeconds();
    if(isAttack)atk = animationTime >= 0.19f && animationTime <= 0.40f;

    // 手の位置
    SkinnedMesh::Animation::Keyframe::Node* rightHand = model->FindNode("joint19");
    DirectX::XMFLOAT4X4 p = rightHand->globalTransform;
    DirectX::XMFLOAT4X4 p2;
    using namespace DirectX;
    XMStoreFloat4x4(&p2, XMLoadFloat4x4(&rightHand->globalTransform) * XMLoadFloat4x4(&transform));
    attackPos = { p2._41, p2._42, p2._43 };

    // 攻撃中は更新しない
    if (!atk && attackCooldown < 0.0f)
    {
        // 攻撃する向きをプレイヤーの方向へ
        if (player->GetCenterPosition().x > position.x) this->direction = false;
        else if (player->GetCenterPosition().x < position.x) this->direction = true;
        // 体の向き
        float vx;
        (this->direction ? vx = -1 : vx = 1);
        angle.y = DirectX::XMConvertToRadians(90 * vx);       
    }
    
    // 攻撃      
    MoveAttack(1.0f);
    // 攻撃クールダウン更新
    AttackCooldownUpdate(elapsedTime);

    if (!model->IsPlayAnimatimon())
    {        
        isAttack = false;
        atk = false;
    }

    // 射程距離外 もしくは、射線が通っていないなら走るステートへ
    if (!model->IsPlayAnimatimon() && (!CheckAttackRange() || AttackRayCheck()))
    {
        isAttack = false;
        atk = false;
        TransitionRunState();
    }
}

// 攻撃クールダウン更新
void EnemyMelee::AttackCooldownUpdate(float elapsedTime)
{
    if (attackCooldown > 0.0f)attackCooldown -= elapsedTime;
}


//吹っ飛びステート遷移
void EnemyMelee::TransitionBlowState()
{
    state = State::Blow;

    blowTimer = 0.3f;

    model->PlayAnimation(static_cast<int>(state), false);

    // 止まる
    moveSpeed = 40;

    // 吹っ飛ばす
    MoveBlow();
}

//吹っ飛びステート更新処理
void EnemyMelee::UpdateBlowState(float elapsedTime)
{
    // 吹き飛ばしタイマー更新
    if (blowTimer > 0.0f)blowTimer -= elapsedTime;
    // 吹っ飛ばしたら死亡ステートへ               
    else TransitionDeathState();

    if (!model->IsPlayAnimatimon()) TransitionDeathState();
}


//死亡ステート遷移
void EnemyMelee::TransitionDeathState()
{
    state = State::Death;
    model->PlayAnimation(static_cast<int>(state), false);
    handle = deadEffect->Play(centerPosition, 1.0f);
    // 止まる
    moveSpeed = 0;
    Move(0.0f, 0.0f, moveSpeed);
    velocity = { 0,0,0 };

    OnDead();
}

//死亡ステート更新処理
void EnemyMelee::UpdateDeathState(float elapsedTime)
{
    // 死亡アニメーション終わったら消滅させる
    if (!model->IsPlayAnimatimon())
    {                
        //Destroy();
    }
}