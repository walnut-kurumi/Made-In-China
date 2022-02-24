#include "Player.h"
#include "Framework.h"
#include "Graphics/Shaders.h"
#include "Input/Input.h"
#include "Camera/CameraManager.h"
#include "StageManager.h"

#include "HitManager.h"

Player::Player(ID3D11Device* device) {

    const char* idle = "Data/Models/Player/Animations/Idle.fbx";
    const char* run = "Data/Models/Player/Animations/Running.fbx";
    const char* walk = "Data/Models/Player/Animations/Walking.fbx";
    const char* jump = "Data/Models/Player/Animations/Jump.fbx";
    const char* attack = "Data/Models/Player/Animations/Attack.fbx";

    //model = new Model(device, "Data/Models/Player/Jummo/Jummo.fbx", true, 0);
    model = new Model(device, "Data/Models/Player/Jummo.fbx", true, 0);
    

    model->LoadAnimation(idle, 0, static_cast<int>(AnimeState::Idle));
    model->LoadAnimation(run, 0, static_cast<int>(AnimeState::Run));
    model->LoadAnimation(walk, 0, static_cast<int>(AnimeState::Walk));
    model->LoadAnimation(jump, 0, static_cast<int>(AnimeState::Jump));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Attack));

    position = { 0.0f, 0.0f, 0.0f };
    waistPos = { 0,3,0 };
    scale = { 0.05f, 0.05f, 0.05f };

    UpdateState[static_cast<int>(AnimeState::Idle)] = &Player::UpdateIdleState;
    UpdateState[static_cast<int>(AnimeState::Run)] = &Player::UpdateRunState;
    UpdateState[static_cast<int>(AnimeState::Walk)] = &Player::UpdateWalkState;
    UpdateState[static_cast<int>(AnimeState::Jump)] = &Player::UpdateJumpState;
    UpdateState[static_cast<int>(AnimeState::Attack)] = &Player::UpdateAttackState;

    TransitionIdleState();

    debugRenderer = std::make_unique<DebugRenderer>(device);
}

Player::~Player() {
    delete model;
}

void Player::Init() {
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

    slowSpeed = 0.35f;
    slow = false;
}

void Player::Update(float elapsedTime) {
    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);

    UpdateSpeed(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

    // ジャンプ入力処理
    InputJump();

    // スロー
    InputSlow();

    //オブジェクト行列更新
    UpdateTransform();
    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);
    //モデル行列更新
    model->UpdateTransform(transform);
}


void Player::Render(ID3D11DeviceContext* dc) {
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
    //model->Begin(dc, Shaders::Ins()->GetRampShader());
    model->Render(dc);


    //// 必要なったら追加
    debugRenderer.get()->DrawSphere(position, 1, Vec4(1, 0, 0, 1));
    if(atk) debugRenderer.get()->DrawSphere(atkPos + position + waistPos, 1, Vec4(1, 1, 0, 1));
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}

void Player::DrawDebugGUI() {
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    //ImGui::PushStyleColor(ImGuiCond_FirstUseEver, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {

            
            
            ImGui::SliderFloat("Position X", &position.x, -2000, 2000);
            ImGui::SliderFloat("Position Y", &position.y, -2000, 2000);
            ImGui::SliderFloat("Position Z", &position.z, -2000, 2000);
            
            
            ImGui::SliderFloat("SpeedMax", &maxMoveSpeed, 0, 100);
            ImGui::SliderFloat("Speed", &moveSpeed, 0, 20);
            
            int a = static_cast<int>(state);
            ImGui::SliderInt("State", &a, 0, static_cast<int>(AnimeState::End));
        }

        ImGui::SliderFloat("Angle X", &angle.y, DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180));

        ImGui::RadioButton("death", deathFlag);


    }
    ImGui::End();
#endif
}

Vec3 Player::GetMoveVec() const {
    // 入力情報を所得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    //float ay = gamePad.GetAxisLY();

    // カメラ方向とスティックの入力値によって進行方向を計算する
    const DirectX::XMFLOAT3& cameraRight = CameraManager::Instance().mainC.GetRight();
    const DirectX::XMFLOAT3& cameraFront = CameraManager::Instance().mainC.GetFront();

    // 移動ベクトルはxz平面に水平なベクトルになるようにする
    // カメラ右方向ベクトルをxz単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        // 単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    // カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        // 単位ベクトル化
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // スティックの水平入力値をカメラ右方向に反映し、
    // スティックの垂直入力値をカメラ前方向に反映し、
    // 進行ベクトルを計算する
    Vec3 vec;
    vec.z = /*cameraFrontZ * ay + */cameraRightZ * ax;
    vec.x = /*cameraFrontX * ay + */cameraRightX * ax;
    // Y軸方向には移動しない
    vec.y = 0.0f;

    return vec;
}

bool Player::InputMove(float elapsedTime) {
    if (deathFlag) return false;
    
    // 進行ベクトル所得
    Vec3 moveVec = GetMoveVec();
    
    // 移動処理
    Move(moveVec.x, moveVec.z, moveSpeed);
    
    // 旋回処理(エイムしてない時だけ)
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    
    // 進行ベクトルがゼロベクトルでない場合は入力された
    return moveVec.x || moveVec.y || moveVec.z;
     
    //// 入力情報を所得
    //GamePad& gamePad = Input::Instance().GetGamePad();
    //float ax = gamePad.GetAxisLX();
    //float ay = gamePad.GetAxisLY();
    //
    //// 右
    //Vec3 cameraRight = VecMath::Normalize(CameraManager::Instance().mainC.GetRight());
    //// 前
    //Vec3 cameraFront = VecMath::Normalize(VecMath::Cross(cameraRight, normal));
    //// 上記の前と右を合体！
    //Vec3 translate = cameraRight * ax + cameraFront * ay;
    //
    //// 移動前を保存
    //posMae = position;
    //position += translate * moveSpeed * elapsedTime;
    //posAto = position;
    //
    //Vec3 moveVec = posAto - posMae;
    //
    ////移動ベクトル
    //DirectX::XMFLOAT3 moveVec = GetMoveVec();
    //Move(moveVec.x, moveVec.z, moveSpeed);
    //Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    //
    //return moveVec.x || moveVec.y || moveVec.z;;
}

// ジャンプ入力処理
void Player::InputJump() {
    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A
        || key.STATE(VK_SPACE)) {
        jumpCount++;
        if (jumpCount <= jumpLimit) {
            Jump(jumpSpeed);
        }
    }
}

void Player::InputSlow() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButton() & GamePad::BTN_RIGHT_SHOULDER) {
        slow = true;
        return;
    }
    slow = false;
}

void Player::InputSB() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    // 武器を持っている場合
    if (weapon) {
        if (gamePad.GetButtonDown() & GamePad::BTN_Y) {
            // 武器を投げる
            weapon = false;
        }
    }
    // 武器を持っていない
    else {
        // 投げた武器の場所にワープ
    }
    

}

bool Player::InputAttack() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    // 武器を持っている　
    if (weapon) {
        // 攻撃
        if (gamePad.GetButtonDown() & GamePad::BTN_X) {

            return true;
        }
    }
    return false;
}

// 待機ステート遷移
void Player::TransitionIdleState() {
    state = AnimeState::Idle;
    model->PlayAnimation(static_cast<int>(state),true);
}

// 待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime) {
    //  移動入力処理
    if (InputMove(elapsedTime)) TransitionWalkState();
    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();
    
    Key& key = Input::Instance().GetKey();
}

// 移動ステートへ遷移
void Player::TransitionWalkState() {
    state = AnimeState::Walk;
    moveSpeed = 10;
    model->PlayAnimation(static_cast<int>(state),true);
}

// 移動ステート更新処理
void Player::UpdateWalkState(float elapsedTime) {
    Key& key = Input::Instance().GetKey();
    //  移動入力処理
    if (!InputMove(elapsedTime)) TransitionIdleState();

    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();

    // 走り入力処理
    if (key.STATE(VK_SHIFT)) TransitionRunState();

    // 回避入力処理
    if (key.STATE(VK_SPACE)) TransitionJumpState();
}

//走るステート遷移
void Player::TransitionRunState() {
    state = AnimeState::Run;
    moveSpeed = 15;
    model->PlayAnimation(static_cast<int>(state), true);
}

//走るステート更新処理
void Player::UpdateRunState(float elapsedTime) {
    Key& key = Input::Instance().GetKey();
    //  移動入力処理
    if (!InputMove(elapsedTime)) TransitionIdleState();

    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();

    // 歩き入力処理
    if (!key.STATE(VK_SHIFT)) TransitionWalkState();

    // 回避入力処理
    if (key.STATE(VK_SPACE)) TransitionJumpState();
}

//回避ステート遷移
void Player::TransitionJumpState() {
    state = AnimeState::Jump;
    model->PlayAnimation(static_cast<int>(state), false);
}

//回避ステート更新処理
void Player::UpdateJumpState(float elapsedTime) {
    // アニメーションが終わるまで回避行動
    if (!model->IsPlayAnimatimon()) {
        // 終わったらアイドル状態へ
        TransitionIdleState();
    }
}

void Player::TransitionAttackState() {
    // 入力情報を所得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    state = AnimeState::Attack;
    // 移動を止める
    velocity = {0, 0, 0};
    Move(0, 0, 0);
    // 重力を止める
    gravFlag = false;

    // 攻撃の場所
    atkPos = { -ax, ay, 0 };
    atkPos = VecMath::Normalize(atkPos) * 5;
    atk = true;

    model->PlayAnimation(static_cast<int>(state), false);
}

void Player::UpdateAttackState(float elapsedTime) {
    // 入力情報を所得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    static bool first = false;
    // 一回目処理
    if (!first) {
        first = true;
        // 左右どっちか
        AttackMove(-ax, ay, 30);
    }

    // アニメーションが終わった最後の処理
    if (!model->IsPlayAnimatimon()) {
        // 終わったらアイドル状態へ
        TransitionIdleState();
        first = false;
        // 重力を再びオン
        gravFlag = true;

        AttackMove(0, 0, 30);

        atkPos = { 0,0,0};
        atk = false;
    }
}

void Player::OnLanding() {
    jumpCount = 0;
}
