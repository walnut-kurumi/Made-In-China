//#include "EnemyGunner.h"
//#include "Framework.h"
//#include "Graphics/Shaders.h"
//#include "StageManager.h"
//
//#include "HitManager.h"
//
//EnemyGunner::EnemyGunner(ID3D11Device* device)
//{
//
//    const char* idle = "Data/Models/Enemy/Animations/Idle.fbx";
//    const char* run = "Data/Models/Enemy/Animations/Running.fbx";
//    const char* walk = "Data/Models/Enemy/Animations/Walking.fbx";
//    const char* jump = "Data/Models/Enemy/Animations/Jump.fbx";
//    
//    model = new Model(device, "Data/Models/Enemy/Jummo.fbx", true, 0);
//
//
//    model->LoadAnimation(idle, 0, static_cast<int>(State::Idle));
//    model->LoadAnimation(run, 0, static_cast<int>(State::Run));
//    model->LoadAnimation(walk, 0, static_cast<int>(State::Walk));
//    model->LoadAnimation(jump, 0, static_cast<int>(State::Jump));
//
//    position = { 0.0f, 0.0f, 0.0f };
//
//    scale = { 0.05f, 0.05f, 0.05f };
//
//    UpdateState[static_cast<int>(State::Idle)] = &EnemyGunner::UpdateIdleState;
//    UpdateState[static_cast<int>(State::Run)] = &EnemyGunner::UpdateRunState;
//    UpdateState[static_cast<int>(State::Walk)] = &EnemyGunner::UpdateWalkState;
//    UpdateState[static_cast<int>(State::Jump)] = &EnemyGunner::UpdateJumpState;
//
//    TransitionIdleState();
//
//    debugRenderer = std::make_unique<DebugRenderer>(device);
//}
//
//EnemyGunner::~EnemyGunner()
//{
//    delete model;
//}
//
//void EnemyGunner::Init()
//{
//    SetPosition({ 0, 1, 0 });
//    posMae = position;
//    posAto = position;
//    angle = { 0,0,0 };
//    transform = {
//        1,0,0,0,
//        0,1,0,0,
//        0,0,1,0,
//        0,0,0,1
//    };
//    normal = { 0,0,0 };
//    velocity = { 0,0,0 };
//    moveSpeed = 10;
//
//    moveVecX = 0.0f;
//    moveVecZ = 0.0f;
//    health = 40;
//
//    isDead = false;
//}
//
//void EnemyGunner::Update(float elapsedTime)
//{
//    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);
//
//    UpdateSpeed(elapsedTime);
//
//    // 無敵時間更新
//    UpdateInvincibleTimer(elapsedTime);
//
//    //オブジェクト行列更新
//    UpdateTransform();
//    // モデルアニメーション更新処理
//    model->UpdateAnimation(elapsedTime);
//    //モデル行列更新
//    model->UpdateTransform(transform);
//}
//
//
//void EnemyGunner::Render(ID3D11DeviceContext* dc)
//{
//    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());    
//    model->Render(dc);
//
//
//    //// 必要なったら追加
//    //debugRenderer.get()->DrawSphere(copos, 4, Vec4(1, 0, 0, 1));
//    //debugRenderer.get()->DrawSphere(copos2, 1.5f, Vec4(1, 0, 0, 1));
//    //debugRenderer.get()->DrawSphere(copos3, 1.5f, Vec4(1, 0, 0, 1));
//    //debugRenderer.get()->DrawSphere(copos4, 1.6f, Vec4(1, 0, 0, 1));
//    //debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
//}
//
//void EnemyGunner::DrawDebugGUI()
//{
////#ifdef USE_IMGUI
////    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
////    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
////    //ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));
////
////    if (ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_None))
////    {
////        // トランスフォーム
////        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
////        {
////
////
////
////            ImGui::SliderFloat("Position X", &position.x, -2000, 2000);
////            ImGui::SliderFloat("Position Y", &position.y, -2000, 2000);
////            ImGui::SliderFloat("Position Z", &position.z, -2000, 2000);
////
////
////            ImGui::SliderFloat("SpeedMax", &maxMoveSpeed, 0, 100);
////            ImGui::SliderFloat("Speed", &moveSpeed, 0, 20);
////
////            int a = static_cast<int>(state);
////            ImGui::SliderInt("State", &a, 0, static_cast<int>(State::End));
////        }
////
////        ImGui::SliderFloat("Angle X", &angle.y, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
////
////        ImGui::RadioButton("death", deathFlag);
////
////
////    }
////    ImGui::End();
////#endif
//}
//
//// 待機ステート遷移
//void EnemyGunner::TransitionIdleState()
//{
//    state = State::Idle;
//    model->PlayAnimation(static_cast<int>(state), true);
//}
//
//// 待機ステート更新処理
//void EnemyGunner::UpdateIdleState(float elapsedTime)
//{   
//    Key& key = Input::Instance().GetKey();
//}
//
//// 移動ステートへ遷移
//void EnemyGunner::TransitionWalkState()
//{
//    state = State::Walk;
//    moveSpeed = 10;
//    model->PlayAnimation(static_cast<int>(state), true);
//}
//
//// 移動ステート更新処理
//void EnemyGunner::UpdateWalkState(float elapsedTime)
//{         
//   TransitionRunState();   
//}
//
////走るステート遷移
//void EnemyGunner::TransitionRunState()
//{
//    state = State::Run;
//    moveSpeed = 15;
//    model->PlayAnimation(static_cast<int>(state), true);
//}
//
////走るステート更新処理
//void EnemyGunner::UpdateRunState(float elapsedTime)
//{
//    Key& key = Input::Instance().GetKey();
//    
//    TransitionIdleState();
//
//    // 歩き入力処理
//    if (!key.STATE(VK_SHIFT)) TransitionWalkState();
// 
//}
