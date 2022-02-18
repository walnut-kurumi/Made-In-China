#include "EnemyGunner.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "StageManager.h"

#include "HitManager.h"

EnemyGunner::EnemyGunner(ID3D11Device* device)
{
    UpdateState[static_cast<int>(State::Idle)] = &EnemyGunner::UpdateIdleState;
    UpdateState[static_cast<int>(State::Run)] = &EnemyGunner::UpdateRunState;
    UpdateState[static_cast<int>(State::Walk)] = &EnemyGunner::UpdateWalkState;
    UpdateState[static_cast<int>(State::Attack)] = &EnemyGunner::UpdateAttackState;
    UpdateState[static_cast<int>(State::Blow)] = &EnemyGunner::UpdateBlowState;
    UpdateState[static_cast<int>(State::Death)] = &EnemyGunner::UpdateDeathState;

    position = { 0.0f, 0.0f, 0.0f };

    scale = { 0.05f, 0.05f, 0.05f };
   
    TransitionIdleState();

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

EnemyGunner::~EnemyGunner()
{
}

void EnemyGunner::Init()
{
    SetPosition({ 0, 1, 0 });
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

void EnemyGunner::Render(ID3D11DeviceContext* dc)
{
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());    
    model->Render(dc);


    //// 必要なったら追加
    //debugRenderer.get()->DrawSphere(copos, 4, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos2, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos3, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos4, 1.6f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}

void EnemyGunner::DrawDebugGUI()
{
//#ifdef USE_IMGUI
//    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
//    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
//    //ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));
//
//    if (ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_None))
//    {
//        // トランスフォーム
//        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
//        {
//
//
//
//            ImGui::SliderFloat("Position X", &position.x, -2000, 2000);
//            ImGui::SliderFloat("Position Y", &position.y, -2000, 2000);
//            ImGui::SliderFloat("Position Z", &position.z, -2000, 2000);
//
//
//            ImGui::SliderFloat("SpeedMax", &maxMoveSpeed, 0, 100);
//            ImGui::SliderFloat("Speed", &moveSpeed, 0, 20);
//
//            int a = static_cast<int>(state);
//            ImGui::SliderInt("State", &a, 0, static_cast<int>(State::End));
//        }
//
//        ImGui::SliderFloat("Angle X", &angle.y, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
//
//        ImGui::RadioButton("death", deathFlag);
//
//
//    }
//    ImGui::End();
//#endif
}


// direction の方向に動く
void EnemyGunner::MoveWalk(bool direction)
{
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
}

// ターンするまでのタイマー更新
void EnemyGunner::IdleTimerUpdate()
{
}


// 移動ステートへ遷移
void EnemyGunner::TransitionWalkState()
{
    state = State::Walk;
    moveSpeed = 10;
    model->PlayAnimation(static_cast<int>(state), true);
}

// 移動ステート更新処理
void EnemyGunner::UpdateWalkState(float elapsedTime)
{   
    // 徘徊
    MoveWalk(direction);

    // プレイヤー発見したら走る
    if (Search())
    {
        TransitionRunState();
    }
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