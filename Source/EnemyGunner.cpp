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
//    // ���G���ԍX�V
//    UpdateInvincibleTimer(elapsedTime);
//
//    //�I�u�W�F�N�g�s��X�V
//    UpdateTransform();
//    // ���f���A�j���[�V�����X�V����
//    model->UpdateAnimation(elapsedTime);
//    //���f���s��X�V
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
//    //// �K�v�Ȃ�����ǉ�
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
////        // �g�����X�t�H�[��
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
//// �ҋ@�X�e�[�g�J��
//void EnemyGunner::TransitionIdleState()
//{
//    state = State::Idle;
//    model->PlayAnimation(static_cast<int>(state), true);
//}
//
//// �ҋ@�X�e�[�g�X�V����
//void EnemyGunner::UpdateIdleState(float elapsedTime)
//{   
//    Key& key = Input::Instance().GetKey();
//}
//
//// �ړ��X�e�[�g�֑J��
//void EnemyGunner::TransitionWalkState()
//{
//    state = State::Walk;
//    moveSpeed = 10;
//    model->PlayAnimation(static_cast<int>(state), true);
//}
//
//// �ړ��X�e�[�g�X�V����
//void EnemyGunner::UpdateWalkState(float elapsedTime)
//{         
//   TransitionRunState();   
//}
//
////����X�e�[�g�J��
//void EnemyGunner::TransitionRunState()
//{
//    state = State::Run;
//    moveSpeed = 15;
//    model->PlayAnimation(static_cast<int>(state), true);
//}
//
////����X�e�[�g�X�V����
//void EnemyGunner::UpdateRunState(float elapsedTime)
//{
//    Key& key = Input::Instance().GetKey();
//    
//    TransitionIdleState();
//
//    // �������͏���
//    if (!key.STATE(VK_SHIFT)) TransitionWalkState();
// 
//}
