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

    const char* idle = "Data/Models/Player/Animations/Idle.fbx";
    const char* run = "Data/Models/Player/Animations/Run.fbx";
    //const char* walk = "Data/Models/Player/Animations/Walking.fbx";
    const char* jump = "Data/Models/Player/Animations/Jump.fbx";
    const char* attack = "Data/Models/Player/Animations/Attack.fbx";

    model = new Model(device, "Data/Models/Player/Player.fbx", true, 0);

    model->LoadAnimation(idle, 0, static_cast<int>(AnimeState::Idle));
    model->LoadAnimation(run, 0, static_cast<int>(AnimeState::Run));
    model->LoadAnimation(jump, 0, static_cast<int>(AnimeState::Jump));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Attack));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Finisher));

    position = { 0.0f, 0.0f, 0.0f };
    waistPos = { 0,3,0 };
    headPos = { 0,6,0 };

    scale = { 0.05f, 0.05f, 0.05f };

    UpdateState[static_cast<int>(AnimeState::Idle)] = &Player::UpdateIdleState;
    UpdateState[static_cast<int>(AnimeState::Run)] = &Player::UpdateRunState;
    UpdateState[static_cast<int>(AnimeState::Jump)] = &Player::UpdateJumpState;
    UpdateState[static_cast<int>(AnimeState::Attack)] = &Player::UpdateAttackState;
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
    maxMoveSpeed = 10;

    jumpSpeed = 110.0f;

    moveVecX = 0.0f;
    moveVecZ = 0.0f;
    health = 40;

    height = 8.0f;

    isDead = false;

    slowSpeed = 0.35f;
    slow = false;

    hitstopSpeed = 0.6f;
    hitstop = false;

    atkRadius = 2;
    atkTimer = 0.0f;

    backDir = 5.0f;

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

    // �V�t�g�u���C�N
    InputSB();
    CollisionSBVsEnemies();
    CollisionSBVsStage();

    atkTimer -= elapsedTime;

    //�I�u�W�F�N�g�s��X�V
    UpdateTransform();
    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);
    //���f���s��X�V
    model->UpdateTransform(transform);


    // �U������
    Key& key = Input::Instance().GetKey();
    XINPUT_VIBRATION vib{};
    vib.wLeftMotorSpeed = 32000;
    vib.wRightMotorSpeed = 32000;
    if (key.STATE('l')) {
        XInputSetState(0, &vib);
    }

    // ���񂾂�
    if (health <= 0)isDead = true;
}


void Player::Render(ID3D11DeviceContext* dc) {
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
    //model->Begin(dc, Shaders::Ins()->GetRampShader());
    model->Render(dc);

    // �e�ە`�揈��
    SBManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());

    centerPosition = position;
    centerPosition.y += height;

    // height
    Vec3 heightPos = position;
    heightPos.y += height;
    debugRenderer.get()->DrawSphere(heightPos, 1, Vec4(0.5f, 1, 0, 1));

    // �K�v�Ȃ�����ǉ�
    debugRenderer.get()->DrawSphere(position, 1, Vec4(1, 0, 0, 1));
    if (atk) debugRenderer.get()->DrawSphere(atkPos + position + waistPos, 1, Vec4(1, 1, 0, 1));
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
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

            ImGui::SliderFloat("Speed", &moveSpeed, 0, 20);

            int a = static_cast<int>(state);
            ImGui::SliderInt("State", &a, 0, static_cast<int>(AnimeState::End));
        }

        ImGui::SliderFloat("Angle X", &angle.y, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));
        ImGui::RadioButton("death", deathFlag);
        ImGui::SliderFloat("Height", &height, 0, 10.0f);
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

    // ���񏈗�(�G�C�����ĂȂ�������)
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    // �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
    return moveVec.x || moveVec.y || moveVec.z;
}

// �W�����v���͏���
bool Player::InputJump() {
    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A /* || key.STATE(VK_SPACE)*/) {
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
    Key& key = Input::Instance().GetKey();
    // �����Ă��
    if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER || key.STATE(VK_SHIFT)) {
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
    if ((!(gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER) || !key.STATE(VK_SHIFT)) && slowTimer < slowMax && slowCT == 0) {
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
    //if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER || key.STATE(VK_SHIFT)) {
    //    slow = true;
    //    return;
    //}
    //slow = false;
}

void Player::InputSB() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    ID3D11Device* device = Graphics::Ins().GetDevice();
    Key& key = Input::Instance().GetKey();
    // ����������Ă���ꍇ
    if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER || (key.TRG('z'))) {
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

                // ����������̏ꏊ�Ƀ��[�v
                position = sb->GetPosition();
                sb->Destroy();
            }
            // ������莝����
            weapon = true;
        }
    }
}

bool Player::InputAttack() {
    // ���͏�������
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // ����������Ă���@
    if (weapon && atkTimer < 0.0f) {
        // �U��
        if (gamePad.GetButtonDown() & GamePad::BTN_X) {
            // �U���̏ꏊ
            Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
            if (ax && ay)atkPos = { -ax, ay, 0 };
            else atkPos = { front.x,0,0 };
            atkPos = VecMath::Normalize(atkPos) * 5;
            atk = true;

            CollisionPanchiVsEnemies();
            CollisionPanchiVsProjectile();

            // �U����CT
            if(!isGround) 
                atkTimer = 0.75f;
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
    // �U�����͏���
    if (InputAttack()) TransitionAttackState();
    // �t�B�j�b�V���[�ւ̈ڍs
    if (finish) TransitionFinisherState();
    // �W�����v���͏���
    if (InputJump()) TransitionJumpState();
    Key& key = Input::Instance().GetKey();
}

//����X�e�[�g�J��
void Player::TransitionRunState() {
    state = AnimeState::Run;
    moveSpeed = 50;
    model->PlayAnimation(static_cast<int>(state), true);
}

//����X�e�[�g�X�V����
void Player::UpdateRunState(float elapsedTime) {
    //  �ړ����͏���
    if (!InputMove(elapsedTime)) TransitionIdleState();
    // �U�����͏���
    if (InputAttack()) TransitionAttackState();
    // �t�B�j�b�V���[�ւ̈ڍs
    if (finish) TransitionFinisherState();
    // �W�����v���͏���
    if (InputJump()) TransitionJumpState();
}

//�W�����v�X�e�[�g�J��
void Player::TransitionJumpState() {
    state = AnimeState::Jump;
    moveSpeed = 45;
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
    // �t�B�j�b�V���[�ւ̈ڍs
    if (finish) TransitionFinisherState();
}

void Player::TransitionAttackState() {
    state = AnimeState::Attack;
    // �ړ����~�߂�
    velocity = { 0, 0, 0 };
    Move(0, 0, 0);
    // �d�͂��~�߂�
    gravFlag = false;

    model->PlayAnimation(static_cast<int>(state), false);
}

void Player::UpdateAttackState(float elapsedTime) {
    // ���͏�������
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    static bool first = false;
    // ���ڏ���
    if (!first) {
        first = true;
        // �n�ʂ������ɐ؂��@�󒆂Ȃ獶�E�̂�
        isGround ? AttackMove(-ax, ay, 30) : AttackMove(-ax, 0, 30);
    }

    // �t�B�j�b�V���[�ւ̈ڍs
    if (finish) TransitionFinisherState();
    
    // �A�j���[�V�������I������Ō�̏���
    if (!model->IsPlayAnimatimon()) {
        // �I�������A�C�h����Ԃ�
        TransitionIdleState();
        first = false;
        // �d�͂��ĂуI��
        gravFlag = true;

        AttackMove(0, 0, 30);

        velocity = {0,0,0};

        atkPos = { 0,0,0 };
        atk = false;
       
        // �q�b�g�X�g�b�v�����
        hitstop = false;

        // �J�����V�F�C�N�i�ȑf�j�����
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);
    }
}

void Player::TransitionFinisherState() {
    // �J��
    state = AnimeState::Finisher;

    model->PlayAnimation(static_cast<int>(state), false);
}

void Player::UpdateFinisherState(float elapsedTime) {
    // �A�j���[�V�������I������Ō�̏���
    if (!model->IsPlayAnimatimon()) {
        // �I�������A�C�h����Ԃ�
        TransitionIdleState();
        // �q�b�g�X�g�b�v�����
        hitstop = false;
        // �t�B�j�b�V���[�I���
        finish = false;
    }
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
                cameraMgr.SetShakeFlag(true);
            }
        }
    }
}

void Player::CollisionPanchiVsProjectile() {
    EnemyBulletManager& enemyBManager = EnemyBulletManager::Instance();
    int enemyBCount = enemyBManager.GetProjectileCount();
    for (int i = 0; i < enemyBCount; ++i) {
        EnemyBullet* enemy = enemyBManager.GetProjectile(i);
        // �Փˏ���
        if (Collision::SphereVsSphere(enemy->GetPosition(), atkPos + position + waistPos, enemy->GetRadius(), atkRadius)) {
            enemy->SetReflectionFlag(true);
            enemy->SetDirection(-enemy->GetDirection());
            // �q�b�g�X�g�b�v
            if (!slow)hitstop = true;
            // �J�����V�F�C�N�i�ȑf�j
            CameraManager& cameraMgr = CameraManager::Instance();
            cameraMgr.SetShakeFlag(true);
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
                    enemy->ApplyDamage(1, 0);
                    // �q�b�g�X�g�b�v
                    if (!slow) hitstop = true;
                }
                // �t�B�j�b�V���[����
                finish = true;
                // ������G�̋߂���
                // ���@�ƓG�̈ʒu���獶�E����@�̂����������苗���Ƀ��[�v�@�����ĎE��
                Vec3 dir = VecMath::Normalize(VecMath::Subtract(position, enemy->GetPosition()));
                dir *= backDir;
                position = enemy->GetPosition() + dir;
                
                /// **********************************
                /// ++++++++++++++++++++++++++++++++++
                ///          ��]�𔽉f������
                /// ++++++++++++++++++++++++++++++++++
                /// **********************************

                // �������
                sb->Destroy();
                // ������莝����
                weapon = true;
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
            // �n�ʂɐڒn���Ă���
            position.x = hit.position.x;
            position.y = hit.position.y;
            position.z = hit.position.z;  
            // ����������̏ꏊ�Ƀ��[�v
            position = sb->GetPosition();
            sb->Destroy();
            // �������Ɏ���
            weapon = true;
        }
    }
}