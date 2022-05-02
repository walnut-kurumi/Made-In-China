#include "Player.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "Input/Input.h"
#include "Camera/CameraManager.h"
#include "StageManager.h"

#include "EnemyManager.h"
#include "EnemyBulletManager.h"

#include "SBManager.h"
#include "SBNormal.h"

#include "HitManager.h"

Player::Player(ID3D11Device* device) {

    const char* idle = "Data/Models/Player/Animations/ver9/Idle.fbx";
    const char* run = "Data/Models/Player/Animations/ver9/Run.fbx";
    const char* jump = "Data/Models/Player/Animations/ver9/Jump.fbx";
    const char* attack = "Data/Models/Player/Animations/ver9/Attack.fbx";

    model = new Model(device, "Data/Models/Player/T8.fbx", true, 0);

    model->LoadAnimation(idle, 0, static_cast<int>(AnimeState::Idle));
    model->LoadAnimation(run, 0, static_cast<int>(AnimeState::Run));
    model->LoadAnimation(jump, 0, static_cast<int>(AnimeState::Jump));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Attack));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Throw));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Finisher));

    position = { 0.0f, 0.0f, 0.0f };
    scale = { 0.05f, 0.05f, 0.05f };


    UpdateState[static_cast<int>(AnimeState::Idle)] = &Player::UpdateIdleState;
    UpdateState[static_cast<int>(AnimeState::Run)] = &Player::UpdateRunState;
    UpdateState[static_cast<int>(AnimeState::Jump)] = &Player::UpdateJumpState;
    UpdateState[static_cast<int>(AnimeState::Attack)] = &Player::UpdateAttackState;
    UpdateState[static_cast<int>(AnimeState::Throw)] = &Player::UpdateSBThrowState;
    UpdateState[static_cast<int>(AnimeState::SB)] = &Player::UpdateSBState;
    UpdateState[static_cast<int>(AnimeState::Finisher)] = &Player::UpdateFinisherState;

    TransitionIdleState();

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

Player::~Player() {
    delete model;
}

void Player::Init() {
    SetPosition({ 0, 1, 0 });
    angle = { 0,0,0 };
    transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    normal = { 0,0,0 };
    velocity = { 0,0,0 };    
    moveSpeed = 50;
    // ���������
    weapon = true;
    // ����p �̂̈ʒu
    waistPos = { 0,3,0 };
    headPos = { 0,6,0 };
    atkPos = { 0,0,0 };    // �U���̈ʒu�͍U�����ɐݒ�
    // �U��
    atkRadius = 2;
    atkTimer = 0.0f;
    atkPower = 5.0f;

    // �W�����v�֘A
    jumpSpeed = 110.0f;
    jumpCount = 0;

    // �X���[���[�V�����֘A
    playbackSpeed = 1.0f;
    slowSpeed = 0.5f;
    slowTimer = slowMax;
    slowCTTimer = CTMax;

    // �q�b�g�X�g�b�v
    hitstopSpeed = 0.6f;

    moveVecX = 0.0f;
    moveVecZ = 0.0f;

    height = 8.0f;

    isDead = false;

    sbSpeed = 3.0f;
    sbSpace = 4.0f;
    sbhit = false;
    sbdir = { 0,0,0 };
    sbPos = { 0,0,0 };

    health = 1;
}
#include <Xinput.h>
void Player::Update(float elapsedTime) {
    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

    UpdateSpeed(elapsedTime);

    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);
   
    // �X���[
    InputSlow(elapsedTime);

    // ���񏈗�
    // �ړ������֌���
    if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;


    CollisionSBVsEnemies();
    CollisionSBVsStage();
    if (atk) CollisionPanchiVsEnemies();
    if (atk) CollisionPanchiVsProjectile();

    atkTimer -= elapsedTime;

    Vibration(elapsedTime);

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();
    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);
    //���f���s��X�V
    model->UpdateTransform(transform);



    // ���񂾂�
    if (health <= 0)isDead = true;
}


void Player::Render(ID3D11DeviceContext* dc) {
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
    model->Render(dc);

    // �e�ە`�揈��
    SBManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());

    centerPosition = position;
    centerPosition.y += height / 2.0f;

#ifdef _DEBUG
    // height
    Vec3 heightPos = position;
    heightPos.y += height;
    debugRenderer.get()->DrawSphere(heightPos, 1, Vec4(0.5f, 1, 0, 1));

    // �K�v�Ȃ�����ǉ�
    debugRenderer.get()->DrawSphere(centerPosition, 1, Vec4(0.5f, 0.5f, 0, 1));
    debugRenderer.get()->DrawSphere(position, 1, Vec4(1, 0, 0, 1));
    debugRenderer.get()->DrawSphere(sbPos, 1, Vec4(1, 1, 0, 1));
    if (atk) debugRenderer.get()->DrawSphere(atkPos + position + waistPos, atkRadius, Vec4(1, 1, 0, 1));
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
#endif
}

void Player::DrawDebugGUI() {
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    //ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None)) {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Position X", &position.x, -300, 300);
            ImGui::SliderFloat("Position Y", &position.y, -200, 200);
            ImGui::SliderFloat("Position Z", &position.z, -300, 300);


            Vec3 p = CameraManager::Instance().GetPos();

            ImGui::SliderFloat("Poion X", &p.x, -300, 300);
            ImGui::SliderFloat("Poion Y", &p.y, -200, 200);
            ImGui::SliderFloat("Poion Z", &p.z, -300, 300);

            int a = static_cast<int>(state);
            ImGui::SliderInt("State", &a, 0, static_cast<int>(AnimeState::End));
        }

        ImGui::RadioButton("death", deathFlag);
        ImGui::SliderFloat("Height", &height, 0, 10.0f);

        ImGui::SliderFloat("maxMove", &moveSpeed, 0, 100.0f);

        Mouse& mouse = Input::Instance().GetMouse();
        float mpx = static_cast<float>(mouse.GetPositionX());
        float mpy = static_cast<float>(mouse.GetPositionY());        
        ImGui::SliderFloat("MousePosX", &mpx, -300, 300);
        ImGui::SliderFloat("MousePosY", &mpy, -200, 200);
    }
    ImGui::End();
#endif
}

Vec3 Player::GetMoveVec() const {
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
    if (cameraRightLength > 0.0f) {
        // �P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    // �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f) {
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

    vec.x = -ax;
    vec.y = 0;
    vec.z = 0;

    return vec;
}

bool Player::InputMove(float elapsedTime) {
    if (deathFlag) return false;

    // �i�s�x�N�g������
    Vec3 moveVec = GetMoveVec();

    // �ړ�����
    Move(moveVec.x, moveVec.z, moveSpeed);

    // �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
    return moveVec.x || moveVec.y || moveVec.z;
}

// �W�����v���͏���
bool Player::InputJump() {
    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        jumpCount++;

        // �󒆂̂Ƃ��͈�񂾂�
        if (!isGround)jumpCount++;
        
        if (jumpCount <= jumpLimit) {
            Jump(jumpSpeed);
            return true;
        }
    }
    return false;
}

void Player::InputSlow(float elapsedTime) {
    GamePad& gamePad = Input::Instance().GetGamePad();
    // �����Ă��
    if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER) {
        slowCT = 0;
        // ���Ԃ������ �X�L���N�����ă^�C�}�[���炷
        if (slowTimer >= 0) {
            slowTimer -= elapsedTime / slowSpeed;   // �X���[���Ԃ���ʏ펞�Ԃ֕ϊ�
            slow = true;
        }
        // ���Ԃ��Ȃ��Ƃ� �N�[���^�C������
        else {
            slowTimer = max(slowTimer, 0);
            slowCT = 1;
            slow = false;
        }
    }
    // �����Ă����
    if (!(gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER) && slowTimer < slowMax && slowCT == 0) {
        slowCT = 1;
        slowCTTimer = CTMax;
        slow = false;
    }
    // �N�[���^�C���� CT�^�C�}�[���炷
    if (slowCT == 1) {
        slow = false;
        // �N�[���^�C�����Ԍo��
        if (slowCTTimer >= 0) {
            slowCTTimer -= elapsedTime;
        }
        // �N�[���^�C���I��
        else {
            slowCTTimer = CTMax;
            slowCT = 2;
        }
    }
    else if (slowCT == 2) {
        slow = false;
        // ���Ԃ������ �^�C�}�[����
        if (slowTimer < slowMax) {
            slowTimer += elapsedTime;
        }
        else {
            slowTimer = min(slowTimer, slowMax);
            slowCT = 0;
        }
    }
}

bool Player::InputSB() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    ID3D11Device* device = Graphics::Ins().GetDevice();
    Key& key = Input::Instance().GetKey();
    Mouse& mouse = Input::Instance().GetMouse();

    // ����������Ă���ꍇ pad
    if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER) {
        if (weapon && 
            (gamePad.GetAxisRX() != 0 || gamePad.GetAxisRY() != 0)) {
            // ����𓊂���
            weapon = false;
            // ����
            SBNormal* sb = new SBNormal(device, &SBManager::Instance());
            // �����A�@���˒n�_
            sb->Launch(VecMath::Normalize(Vec3(-gamePad.GetAxisRX(), gamePad.GetAxisRY(), 0)), position + waistPos);
        }
        // ����������Ă��Ȃ�
        else if(!weapon) {
            // SB�T��
            SBManager& sbManager = SBManager::Instance();
            int sbCount = sbManager.GetProjectileCount();
            for (int i = 0; i < sbCount; ++i) {
                SB* sb = sbManager.GetProjectile(i);
                // ������ݒ�
                direction = VecMath::sign(sb->GetPosition().x - position.x);
                // ���񏈗�
                if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                // ���[�v��w��
                sbdir = VecMath::Normalize(sb->GetPosition() - position);
                sbPos = sb->GetPosition();
                sbhit = true;
                sb->Destroy();
            }
        }
        return true;
    }

    // ����������Ă���ꍇ mouse
    if (mouse.GetButtonDown() & Mouse::BTN_RIGHT) {
        if (weapon) {
            // ����𓊂���
            weapon = false;
            // ����
            SBNormal* sb = new SBNormal(device, &SBManager::Instance());

            // �}�E�X�J�[�\�����W���擾            
            Vec3 cursor;
            cursor.x = static_cast<float>(mouse.GetPositionX());
            cursor.y = static_cast<float>(mouse.GetPositionY());
            cursor.z = 0.0f;
            // player pos
            Vec3 playerScreenPos;
            playerScreenPos = { Graphics::Ins().GetScreenWidth() * 0.5f, Graphics::Ins().GetScreenHeight() * 0.5f + 3.0f, 0.0f };

            // ����
            Vec3 atkPos = playerScreenPos - cursor;
            atkPos = VecMath::Normalize(atkPos);            

            // ���˒n�_
            sb->Launch(atkPos, position + waistPos);
        }
        // ����������Ă��Ȃ�
        else if (!weapon) {
            // SB�T��
            SBManager& sbManager = SBManager::Instance();
            int sbCount = sbManager.GetProjectileCount();
            for (int i = 0; i < sbCount; ++i) {
                SB* sb = sbManager.GetProjectile(i);

                // ������ݒ�
                direction = VecMath::sign(sb->GetPosition().x - position.x);
                // ���񏈗�
                if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                // ����������̏ꏊ�Ƀ��[�v
                // ���[�v��w��
                sbdir = VecMath::Normalize(sb->GetPosition() - position);
                sbPos = sb->GetPosition();
                sbhit = true;
                sb->Destroy();
            }
        }
        return true;
    }
    return false;
}

bool Player::InputAttack() {
    // ���͏�������
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();
    Mouse& mouse = Input::Instance().GetMouse();

    // ����������Ă���@
    if (weapon && atkTimer < 0.0f) {
        // �U��
        if (gamePad.GetButtonDown() & GamePad::BTN_X) {
            // �U���̏ꏊ
            Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
            if (ax && ay)atkPos = { -ax, ay, 0 };
            else atkPos = { front.x,0,0 };
            atkPos = VecMath::Normalize(atkPos) * 3;

            // �U����CT
            if(!isGround) atkTimer = 0.50f;
            return true;
        }
        else if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
            
            // �}�E�X�J�[�\�����W���擾            
            Vec3 cursor;                    
            cursor.x = static_cast<float>(mouse.GetPositionX());
            cursor.y = static_cast<float>(mouse.GetPositionY());
            cursor.z = 0.0f;
            
            Vec3 playerScreenPos;
            playerScreenPos = { Graphics::Ins().GetScreenWidth() * 0.5f, Graphics::Ins().GetScreenHeight() * 0.5f + 3.0f, 0.0f };
            
            atkPos = playerScreenPos - cursor;
            atkPos = VecMath::Normalize(atkPos) * 5;

            // �U����CT
            if (!isGround) atkTimer = 0.50f;
            return true;
        }
    }
    return false;
}

// �ҋ@�X�e�[�g�J��
void Player::TransitionIdleState() {
    state = AnimeState::Idle;
    model->PlayAnimation(static_cast<int>(state), true);
}
// �ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime) {
    //  �ړ����͏���
    if (InputMove(elapsedTime)) TransitionRunState();
    // �W�����v���͏���
    if (InputJump()) TransitionJumpState();
    // �U�����͏���
    if (InputAttack()) TransitionAttackState();
    // �V�t�g�u���C�N
    if (InputSB()) TransitionSBThrowState();
}

//����X�e�[�g�J��
void Player::TransitionRunState() {
    state = AnimeState::Run;
    model->PlayAnimation(static_cast<int>(state), true);
}
//����X�e�[�g�X�V����
void Player::UpdateRunState(float elapsedTime) {
    //  �ړ����͏���
    if (!InputMove(elapsedTime)) TransitionIdleState();
    // �U�����͏���
    if (InputAttack()) TransitionAttackState();
    // �W�����v���͏���
    if (InputJump()) TransitionJumpState();
    // �V�t�g�u���C�N
    if (InputSB()) TransitionSBThrowState();
}

//�W�����v�X�e�[�g�J��
void Player::TransitionJumpState() {
    state = AnimeState::Jump;
    model->PlayAnimation(static_cast<int>(state), false);
}
//�W�����v�X�e�[�g�X�V����
void Player::UpdateJumpState(float elapsedTime) {
    //  �ړ����͏���
    InputMove(elapsedTime);
    // �U�����͏���
    if (InputAttack()) TransitionAttackState();
    // �W�����v���͏���
    if (InputJump()) TransitionJumpState();
    // �n�ʂɂ�����A�C�h����Ԃ�
    if (isGround) TransitionIdleState();
    // �V�t�g�u���C�N
    if (InputSB()) TransitionSBThrowState();
}

void Player::TransitionAttackState() {
    state = AnimeState::Attack;
    model->PlayAnimation(static_cast<int>(state), false);
    //�ړ��͂��U���ɏ悹�Ȃ�
    velocity = {0,0,0};
}
void Player::UpdateAttackState(float elapsedTime) {
    static bool first = false;
    if (!first) {
        // �U���̌����w��
        Vec3 atkMove = atkPos * atkPower;
        if (!Ground()) {
            // �󒆂ɂ���Ƃ��̍U���͏�ɔ�΂Ȃ�
            atkMove.y = 0;
            // �󒆍U���̏d�͂͌y���؋󂳂���
            gravity = -0.0f;
        }
        else {
            // �n�ʂ���󒆂Ɍ����čU�����Ă�Ƃ�
            if (atkMove.y > 0) {
                // ���C����߂邽�߂ɋ󒆂ɂ���
                position.y += 0.01f;
                isGround = false;
            }
        }
        // �U�������ւƂ΂�
        AddImpulse(atkMove);
        first = true;
    }

    if (Ground()) velocity.x = 0;

    // �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    float animationTime = model->GetCurrentAnimationSeconds();
    atk = animationTime >= 0.01f && animationTime <= 0.20f;
    
    // �A�j���[�V�������I������Ō�̏���
    if (!model->IsPlayAnimatimon()) {
        // �I�������A�C�h����Ԃ�
        TransitionIdleState();

        // �d�͂��ĂуI��
        gravFlag = true;

        // �U���ʒu���Z�b�g
        atkPos = { 0,0,0 };
        atk = false;
       
        // �q�b�g�X�g�b�v�����
        hitstop = false;

        // �d�͖߂�
        gravity = -3.0f;

        // �J�����V�F�C�N�i�ȑf�j�����
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);
        first = false;
    }
}

void Player::TransitionSBThrowState() {
    // �J��
    state = AnimeState::Throw;

    model->PlayAnimation(static_cast<int>(state), false);
    // SB���d�͂Ȃ�
    gravFlag = false;
    velocity = { 0,0,0 };
}
void Player::UpdateSBThrowState(float elapsedTime) {
    // SB���������玟�̃X�e�[�g��
    if (InputSB()) TransitionSBState();
    // �����ɓ��������甭��
    if(sbhit) TransitionSBState();
}

void Player::TransitionSBState() {
    // �J��
    state = AnimeState::SB;
    sbhit = false;
    clock = true;
    // �A�j���[�V�����͎~�߂�
    //model->PlayAnimation(static_cast<int>(state), false);
}
void Player::UpdateSBState(float elapsedTime) {
    // �ړ��{���C�L���X�g
    if(Raycast(sbdir * sbSpeed)) {
        sbPos = { 0,0,0 };
        TransitionFinisherState();
    }

    //position += sbdir * sbSpeed;

    // �G�ɓ��B������SB�U���X�e�[�g��
    if (VecMath::LengthVec3(sbPos - position) <= sbSpace) {
        position = sbPos;
        sbPos = { 0,0,0 };
        TransitionFinisherState();
    }
}

void Player::TransitionFinisherState() {
    // �J��
    state = AnimeState::Finisher;
    model->PlayAnimation(static_cast<int>(state), false);
    // �q�b�g�X�g�b�v
    if (!slow) hitstop = true;
    // �J�����V�F�C�N�i�ȑf�j
    CameraManager& cameraMgr = CameraManager::Instance();
    if (!cameraMgr.GetShakeFlag()) {
        cameraMgr.SetShakeFlag(true);
        vibration = true;
        vibTimer = 0.4f;
    }

    // �U���̏ꏊ
    Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
    atkPos = { front.x,0,0 };
    atkPos = VecMath::Normalize(atkPos) * 3;
}
void Player::UpdateFinisherState(float elapsedTime) {
    // �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    float animationTime = model->GetCurrentAnimationSeconds();
    atk = animationTime >= 0.01f && animationTime <= 0.20f;

    // �A�j���[�V�������I������Ō�̏���
    if (!model->IsPlayAnimatimon()) {
        // �I�������A�C�h����Ԃ�
        TransitionIdleState();
        // �q�b�g�X�g�b�v�����
        hitstop = false;
        // �������Ɏ���
        weapon = true;
        //�d�͕���
        gravFlag = true;
        // ���Ԗ߂�
        clock = false;
        // �U���ʒu���Z�b�g
        atkPos = { 0,0,0 };
        atk = false;
        // �J�����V�F�C�N�i�ȑf�j�����
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);
    }
}


void Player::Vibration(float elapsedTime) {
    //// �U������
    //Key& key = Input::Instance().GetKey();
    //XINPUT_VIBRATION vib{};
    //XINPUT_VIBRATION vib2{};
    //{
    //    vib.wLeftMotorSpeed = MAX_SPEED;
    //    vib.wRightMotorSpeed = MIN_SPEED;
    //    vib2.wLeftMotorSpeed = 0;
    //    vib2.wRightMotorSpeed = 0;
    //}
    //XInputSetState(0, &vib);
    //if (vibration && vibTimer >= 0.0f) {
    //    if (slow) vibTimer -= elapsedTime / slowSpeed;
    //    else vibTimer -= elapsedTime;
    //}
    //else {
    //    XInputSetState(0, &vib2);
    //}
}

bool Player::Raycast(Vec3 move) {

    // �ړ���
    float my = move.y;
    bool result = false;

    // ������
    if (my < 0.0f) {
        // ���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // ���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        // ���C�L���X�g�ɂ��n�ʔ���
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {

            // �n�ʂɐڒn���Ă���
            position.x = hit.position.x;
            position.y = hit.position.y;
            position.z = hit.position.z;

            // ��]
            angle.y += hit.rotation.y;

            //  �X�Η��̌v�Z
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            // ���n����
            if (!isGround) OnLanding();

            isGround = true;
            velocity.y = 0.0f;
            result = true;
        }
        else {
            // �󒆂ɕ����Ă�
            position += move;
            isGround = false;
        }
    }
    // �㏸��
    else if (my > 0.0f) {

        // ���C�̊J�n�ʒu�͓�
        DirectX::XMFLOAT3 start = { position.x, position.y + height, position.z };
        // ���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = { position.x, position.y + height + my, position.z };

        // ���C�L���X�g�ɂ��V�䔻��
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {

            // �V��ɐڂ��Ă���
            position.x = hit.position.x;
            position.y = hit.position.y - height;
            position.z = hit.position.z;

            // ��]
            angle.y += hit.rotation.y;

            velocity.y = 0.0f;
            result = true;
        }
        else {
            // �󒆂ɕ����Ă�
            position += move;
        }
    }
    else {
        position += move;
    }



    // �������͌v�Z
    float velocityLengthXZ = sqrtf(move.x * move.x + move.z * move.z);
    // �ړ���������
    direction = VecMath::sign(velocity.x);

    if (velocityLengthXZ > 0.0f) {
        // �����ړ��l
        float mx = move.x;

        // ���C�̊J�n�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x , position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z };
        // ���C�̊J�n�ʒu�ƏI�_�ʒu(��)
        DirectX::XMFLOAT3 start2 = { position.x , position.y + stepOffset + height, position.z };
        DirectX::XMFLOAT3 end2 = { position.x + mx, position.y + stepOffset + height, position.z };

        // ���C�L���X�g�ɂ��ǔ���
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            // �ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // �ǂ̖@��
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // ���˃x�N�g����@���Ɏˉe                          // �x�N�g���̔ے肷��
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // �␳�ʒu�̌v�Z                   // v1 �x�N�g���搔 v2 �x�N�^�[��Z   3 �Ԗڂ̃x�N�g���ɒǉ����ꂽ�ŏ��� 2 �̃x�N�g���̐ς��v�Z
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // �߂�l �x�N�g���̐Ϙa��Ԃ�    �߂�l�@���@v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // �␳�ʒu���ǂɖ��܂��Ă��邩�ǂ���
            if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2)) {
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
            result = true;
        }
        else if (StageManager::Instance().RayCast(start2, end2, hit)) {
            // �ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start2);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end2);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // �ǂ̖@��
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // ���˃x�N�g����@���Ɏˉe                          // �x�N�g���̔ے肷��
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // �␳�ʒu�̌v�Z                   // v1 �x�N�g���搔 v2 �x�N�^�[��Z   3 �Ԗڂ̃x�N�g���ɒǉ����ꂽ�ŏ��� 2 �̃x�N�g���̐ς��v�Z
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // �߂�l �x�N�g���̐Ϙa��Ԃ�    �߂�l�@���@v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // �␳�ʒu���ǂɖ��܂��Ă��邩�ǂ���
            if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2)) {
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
            result = true;
        }
    }


    return result;
}

void Player::OnLanding() {
    jumpCount = 0;
}

// �e�ۂƓG�̏Փ˔���
void Player::CollisionPanchiVsEnemies() {
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i) {
        Enemy* enemy = enemyManager.GetEnemy(i);
        // �Փˏ���
        if (Collision::SphereVsSphere(enemy->GetPosition(), atkPos + position + waistPos, enemy->GetRadius(), atkRadius)) {        
            if (enemy->GetHealth() > 0) {
                enemy->ApplyDamage(1, 0);
                // �q�b�g�X�g�b�v
                if (!slow)hitstop = true;

                // �J�����V�F�C�N�i�ȑf�j
                CameraManager& cameraMgr = CameraManager::Instance();
                if (!cameraMgr.GetShakeFlag()) {
                    cameraMgr.SetShakeFlag(true);
                    vibration = true;
                    vibTimer = 0.4f;
                }
            }
        }
    }
}

void Player::CollisionPanchiVsProjectile() {
    EnemyBulletManager& enemyBManager = EnemyBulletManager::Instance();
    int enemyBCount = enemyBManager.GetProjectileCount();
    for (int i = 0; i < enemyBCount; ++i) {
        EnemyBullet* enemyBullet = enemyBManager.GetProjectile(i);
        // �Փˏ���
        if (Collision::SphereVsSphere(enemyBullet->GetPosition(), atkPos + position + waistPos, enemyBullet->GetRadius(), atkRadius)) {
            if(!enemyBullet->GetReflectionFlag()) enemyBullet->SetDirection(-enemyBullet->GetDirection());
            enemyBullet->SetReflectionFlag(true);
            // �q�b�g�X�g�b�v
            if (!slow)hitstop = true;
            // �J�����V�F�C�N�i�ȑf�j
            CameraManager& cameraMgr = CameraManager::Instance();
            if (!cameraMgr.GetShakeFlag()) {
                cameraMgr.SetShakeFlag(true);
                vibration = true;
                vibTimer = 0.4f;
            }
        }
    }
}

// �G��SB
void Player::CollisionSBVsEnemies() {
    // �G�T��
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i) {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // SB�T��
        SBManager& sbManager = SBManager::Instance();
        int enemyBCount = sbManager.GetProjectileCount();
        for (int i = 0; i < enemyBCount; ++i) {
            SB* sb = sbManager.GetProjectile(i);

            // �Փˏ���
            if (Collision::SphereVsSphere(enemy->GetPosition(), sb->GetPosition(), enemy->GetRadius(), atkRadius)) {

                if (enemy->GetHealth() > 0) {


                    // ������ݒ�
                    direction = VecMath::sign(enemy->GetPosition().x - position.x);
                    // ���񏈗�
                    if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                    // �G��SB�q�b�g
                    sbhit = true;

                    // ������G�̋߂���
                    // ���@�ƓG�̈ʒu���獶�E����@�̂����������苗���Ƀ��[�v�@�����ĎE��
                    sbdir = VecMath::Normalize(VecMath::Subtract(enemy->GetPosition(), position));
                    // ���B�n�_
                    // �G�́A�������ʂɈ�苗��
                    sbPos = enemy->GetPosition();
                    sbPos.x += -(VecMath::sign(enemy->GetPosition().x - position.x)) * sbSpace;

                    //Vec3 dir = VecMath::Normalize(VecMath::Subtract(position, enemy->GetPosition()));
                    //dir *= backDir;
                    //position = enemy->GetPosition() + dir;
                    //enemy->ApplyDamage(1, 0);

                    // �������
                    sb->Destroy();
                }
            }
        }
    }
}

void Player::CollisionSBVsStage() {
    // SB�T��
    SBManager& sbManager = SBManager::Instance();
    int sbCount = sbManager.GetProjectileCount();
    for (int i = 0; i < sbCount; ++i) {
        // SB���
        SB* sb = sbManager.GetProjectile(i);
        Vec3 pos = sb->GetPosition();
        Vec3 dir = sb->GetDirection();
        float speed = sb->GetSpeed();

        HitResult hit;
        // �X�e�[�W�Ƃ̔���
        if (StageManager::Instance().RayCast(pos, pos + VecMath::Normalize(dir) * speed, hit)) {
            // ������ݒ�
            direction = VecMath::sign(hit.position.x - position.x);
            // ���񏈗�
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;

            // ������G�̋߂���
            sbdir = VecMath::Normalize(VecMath::Subtract(hit.position, position));
            // ���B�n�_
            // �G�́A�������ʂɈ�苗��
            sbPos = hit.position;


            sb->Destroy();
            // stage��SB�q�b�g
            sbhit = true;
        }
    }
}