#include "Player.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "Input/Input.h"
#include "Camera/CameraManager.h"
#include "StageManager.h"

#include "HitManager.h"

Player::Player(ID3D11Device* device)
{

    const char* idle = "Data/Models/Player/Animations/Idle.fbx";
    const char* run = "Data/Models/Player/Animations/Running.fbx";
    const char* walk = "Data/Models/Player/Animations/Walking.fbx";
    const char* jump = "Data/Models/Player/Animations/Jump.fbx";

    //model = new Model(device, "Data/Models/Player/Jummo/Jummo.fbx", true, 0);
    model = new Model(device, "Data/Models/Player/Jummo.fbx", true, 0);
    

    model->LoadAnimation(idle, 0, static_cast<int>(State::Idle));
    model->LoadAnimation(run, 0, static_cast<int>(State::Run));
    model->LoadAnimation(walk, 0, static_cast<int>(State::Walk));
    model->LoadAnimation(jump, 0, static_cast<int>(State::Jump));

    position = { 0.0f, 0.0f, 0.0f };

    scale = { 0.05f, 0.05f, 0.05f };

    UpdateState[static_cast<int>(State::Idle)] = &Player::UpdateIdleState;
    UpdateState[static_cast<int>(State::Run)] = &Player::UpdateRunState;
    UpdateState[static_cast<int>(State::Walk)] = &Player::UpdateWalkState;
    UpdateState[static_cast<int>(State::Jump)] = &Player::UpdateJumpState;

    TransitionIdleState();

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

Player::~Player()
{
    delete model;
}

void Player::Init()
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

void Player::Update(float elapsedTime)
{
    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

    UpdateSpeed(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // �W�����v���͏���
    InputJump();

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();
    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);
    //���f���s��X�V
    model->UpdateTransform(transform);
}


void Player::Render(ID3D11DeviceContext* dc)
{
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
    //model->Begin(dc, Shaders::Ins()->GetRampShader());
    model->Render(dc);


    //// �K�v�Ȃ�����ǉ�
    //debugRenderer.get()->DrawSphere(copos, 4, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos2, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos3, 1.5f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->DrawSphere(copos4, 1.6f, Vec4(1, 0, 0, 1));
    //debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}

void Player::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    //ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {

            
            
            ImGui::SliderFloat("Position X", &position.x, -2000, 2000);
            ImGui::SliderFloat("Position Y", &position.y, -2000, 2000);
            ImGui::SliderFloat("Position Z", &position.z, -2000, 2000);
            
            
            ImGui::SliderFloat("SpeedMax", &maxMoveSpeed, 0, 100);
            ImGui::SliderFloat("Speed", &moveSpeed, 0, 20);
            
            int a = static_cast<int>(state);
            ImGui::SliderInt("State", &a, 0, static_cast<int>(State::End));
        }

        ImGui::SliderFloat("Angle X", &angle.y, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));

        ImGui::RadioButton("death", deathFlag);


    }
    ImGui::End();
#endif
}

Vec3 Player::GetMoveVec() const
{
    // ���͏�������
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    const DirectX::XMFLOAT3& cameraRight = CameraManager::Instance().mainC.GetRight();
    const DirectX::XMFLOAT3& cameraFront = CameraManager::Instance().mainC.GetFront();

    // �ړ��x�N�g����xz���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
    // �J�����E�����x�N�g����xz�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        // �P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    // �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        // �P�ʃx�N�g����
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    // �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    // �i�s�x�N�g�����v�Z����
    Vec3 vec;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    // Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;

    return vec;
}

bool Player::InputMove(float elapsedTime)
{
    if (deathFlag) return false;
    
    // �i�s�x�N�g������
    Vec3 moveVec = GetMoveVec();
    
    // �ړ�����
    Move(moveVec.x, moveVec.z, moveSpeed);
    
    // ���񏈗�(�G�C�����ĂȂ�������)
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    
    // �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
    return moveVec.x || moveVec.y || moveVec.z;
     
    //// ���͏�������
    //GamePad& gamePad = Input::Instance().GetGamePad();
    //float ax = gamePad.GetAxisLX();
    //float ay = gamePad.GetAxisLY();
    //
    //// �E
    //Vec3 cameraRight = VecMath::Normalize(CameraManager::Instance().mainC.GetRight());
    //// �O
    //Vec3 cameraFront = VecMath::Normalize(VecMath::Cross(cameraRight, normal));
    //// ��L�̑O�ƉE�����́I
    //Vec3 translate = cameraRight * ax + cameraFront * ay;
    //
    //// �ړ��O��ۑ�
    //posMae = position;
    //position += translate * moveSpeed * elapsedTime;
    //posAto = position;
    //
    //Vec3 moveVec = posAto - posMae;
    //
    ////�ړ��x�N�g��
    //DirectX::XMFLOAT3 moveVec = GetMoveVec();
    //Move(moveVec.x, moveVec.z, moveSpeed);
    //Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    //
    //return moveVec.x || moveVec.y || moveVec.z;;
}

// �W�����v���͏���
void Player::InputJump() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        jumpCount++;
        if (jumpCount <= jumpLimit) {
            Jump(jumpSpeed);
        }
    }
}

// �ҋ@�X�e�[�g�J��
void Player::TransitionIdleState()
{
    state = State::Idle;
    model->PlayAnimation(static_cast<int>(state),true);
}

// �ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime)
{
    //  �ړ����͏���
    if (InputMove(elapsedTime)) TransitionWalkState();
    
    Key& key = Input::Instance().GetKey();
}

// �ړ��X�e�[�g�֑J��
void Player::TransitionWalkState()
{
    state = State::Walk;
    moveSpeed = 10;
    model->PlayAnimation(static_cast<int>(state),true);
}

// �ړ��X�e�[�g�X�V����
void Player::UpdateWalkState(float elapsedTime)
{
    Key& key = Input::Instance().GetKey();
    //  �ړ����͏���
    if (!InputMove(elapsedTime)) TransitionIdleState();

    // ������͏���
    if (key.STATE(VK_SHIFT)) TransitionRunState();

    // �����͏���
    if (key.STATE(VK_SPACE)) TransitionJumpState();
}

//����X�e�[�g�J��
void Player::TransitionRunState()
{
    state = State::Run;
    moveSpeed = 15;
    model->PlayAnimation(static_cast<int>(state), true);
}

//����X�e�[�g�X�V����
void Player::UpdateRunState(float elapsedTime)
{
    Key& key = Input::Instance().GetKey();
    //  �ړ����͏���
    if (!InputMove(elapsedTime)) TransitionIdleState();

    // �������͏���
    if (!key.STATE(VK_SHIFT)) TransitionWalkState();

    // �����͏���
    if (key.STATE(VK_SPACE)) TransitionJumpState();
}

//����X�e�[�g�J��
void Player::TransitionJumpState()
{
    state = State::Jump;
    model->PlayAnimation(static_cast<int>(state), false);
}

//����X�e�[�g�X�V����
void Player::UpdateJumpState(float elapsedTime)
{
    // ���͏�������
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    static bool first = false;
    static float leftRight = 0.0f;
    static float backFront = 0.0f;
    // ���ڏ���
    if (!first) {
        first = true;
        // ���E�ǂ�����
        leftRight = ax;
        backFront = ay;
    }
    // �A�j���[�V�������I���܂ŉ���s��
    if (!model->IsPlayAnimatimon()) {
        // �I�������A�C�h����Ԃ�
        TransitionIdleState();
        first = false;
        leftRight = 0;
        backFront = 0;
    }



}

void Player::OnLanding() {
    jumpCount = 0;
}
