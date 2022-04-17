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

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);
   
    // スロー
    InputSlow(elapsedTime);

    // シフトブレイク
    InputSB();
    CollisionSBVsEnemies();
    CollisionSBVsStage();

    atkTimer -= elapsedTime;

    //オブジェクト行列更新
    UpdateTransform();
    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);
    //モデル行列更新
    model->UpdateTransform(transform);


    // 振動試し
    Key& key = Input::Instance().GetKey();
    XINPUT_VIBRATION vib{};
    vib.wLeftMotorSpeed = 32000;
    vib.wRightMotorSpeed = 32000;
    if (key.STATE('l')) {
        XInputSetState(0, &vib);
    }

    // 死んだら
    if (health <= 0)isDead = true;
}


void Player::Render(ID3D11DeviceContext* dc) {
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
    //model->Begin(dc, Shaders::Ins()->GetRampShader());
    model->Render(dc);

    // 弾丸描画処理
    SBManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());

    centerPosition = position;
    centerPosition.y += height;

    // height
    Vec3 heightPos = position;
    heightPos.y += height;
    debugRenderer.get()->DrawSphere(heightPos, 1, Vec4(0.5f, 1, 0, 1));

    // 必要なったら追加
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
        // トランスフォーム
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
    // 入力情報を所得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // カメラ方向とスティックの入力値によって進行方向を計算する
    const DirectX::XMFLOAT3& cameraRight = CameraManager::Instance().mainC.GetRight();
    const DirectX::XMFLOAT3& cameraFront = CameraManager::Instance().mainC.GetFront();

    // 移動ベクトルはxz平面に水平なベクトルになるようにする
    // カメラ右方向ベクトルをxz単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f) {
        // 単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    // カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f) {
        // 単位ベクトル化
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // スティックの水平入力値をカメラ右方向に反映し、
    // スティックの垂直入力値をカメラ前方向に反映し、
    // 進行ベクトルを計算する
    Vec3 vec;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    // Y軸方向には移動しない
    vec.y = 0.0f;

    vec.x = -ax;
    vec.y = 0;
    vec.z = 0;

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
}

// ジャンプ入力処理
bool Player::InputJump() {
    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A /* || key.STATE(VK_SPACE)*/) {
        jumpCount++;

        // 空中のときは一回だけ
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
    // 押してる間
    if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER || key.STATE(VK_SHIFT)) {
        slowCT = 0;
        // 時間があれば スキル起動してタイマー減らす
        if (slowTimer >= 0) {
            slowTimer -= elapsedTime / slowSpeed;   // スロー時間から通常時間へ変換
            slow = true;
        }
        // 時間がないとき クールタイム発動
        else {
            slowTimer = max(slowTimer, 0);
            slowCT = 1;
            slow = false;
        }
    }
    // 放している間
    if ((!(gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER) || !key.STATE(VK_SHIFT)) && slowTimer < slowMax && slowCT == 0) {
        slowCT = 1;
        slowCTTimer = CTMax;
        slow = false;
    }
    // クールタイム中 CTタイマー減らす
    if (slowCT == 1) {
        slow = false;
        // クールタイム時間経過
        if (slowCTTimer >= 0) {
            slowCTTimer -= elapsedTime;
        }
        // クールタイム終了
        else {
            slowCTTimer = CTMax;
            slowCT = 2;
        }
    }
    else if (slowCT == 2) {
        slow = false;
        // 時間があれば タイマー復活
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
    // 武器を持っている場合
    if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER || (key.TRG('z'))) {
        if (weapon && 
            (gamePad.GetAxisRX() != 0 || gamePad.GetAxisRY() != 0)) {
            // 武器を投げる
            weapon = false;
            // 発射
            SBNormal* sb = new SBNormal(device, &SBManager::Instance());
            // 向き、　発射地点
            sb->Launch(VecMath::Normalize(Vec3(-gamePad.GetAxisRX(), gamePad.GetAxisRY(), 0)), position + waistPos);
        }
        // 武器を持っていない
        else if(!weapon) {
            // SB探索
            SBManager& sbManager = SBManager::Instance();
            int sbCount = sbManager.GetProjectileCount();
            for (int i = 0; i < sbCount; ++i) {
                SB* sb = sbManager.GetProjectile(i);

                // 投げた武器の場所にワープ
                position = sb->GetPosition();
                sb->Destroy();
            }
            // 武器を手持ちに
            weapon = true;
        }
    }
}

bool Player::InputAttack() {
    // 入力情報を所得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // 武器を持っている　
    if (weapon && atkTimer < 0.0f) {
        // 攻撃
        if (gamePad.GetButtonDown() & GamePad::BTN_X) {
            // 攻撃の場所
            Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
            if (ax && ay)atkPos = { -ax, ay, 0 };
            else atkPos = { front.x,0,0 };
            atkPos = VecMath::Normalize(atkPos) * 5;
            atk = true;

            CollisionPanchiVsEnemies();
            CollisionPanchiVsProjectile();

            // 攻撃のCT
            if(!isGround) 
                atkTimer = 0.75f;
            return true;
        }        
    }
    return false;
}

// 待機ステート遷移
void Player::TransitionIdleState() {
    state = AnimeState::Idle;
    model->PlayAnimation(static_cast<int>(state), true);
}

// 待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime) {
    //  移動入力処理
    if (InputMove(elapsedTime)) TransitionRunState();
    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();
    // フィニッシャーへの移行
    if (finish) TransitionFinisherState();
    // ジャンプ入力処理
    if (InputJump()) TransitionJumpState();
    Key& key = Input::Instance().GetKey();
}

//走るステート遷移
void Player::TransitionRunState() {
    state = AnimeState::Run;
    moveSpeed = 50;
    model->PlayAnimation(static_cast<int>(state), true);
}

//走るステート更新処理
void Player::UpdateRunState(float elapsedTime) {
    //  移動入力処理
    if (!InputMove(elapsedTime)) TransitionIdleState();
    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();
    // フィニッシャーへの移行
    if (finish) TransitionFinisherState();
    // ジャンプ入力処理
    if (InputJump()) TransitionJumpState();
}

//ジャンプステート遷移
void Player::TransitionJumpState() {
    state = AnimeState::Jump;
    moveSpeed = 45;
    model->PlayAnimation(static_cast<int>(state), false);
}

//ジャンプステート更新処理
void Player::UpdateJumpState(float elapsedTime) {
    //  移動入力処理
    InputMove(elapsedTime);
    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();
    // ジャンプ入力処理
    if (InputJump()) TransitionJumpState();
    // 地面についたらアイドル状態へ
    if (isGround) TransitionIdleState();
    // フィニッシャーへの移行
    if (finish) TransitionFinisherState();
}

void Player::TransitionAttackState() {
    state = AnimeState::Attack;
    // 移動を止める
    velocity = { 0, 0, 0 };
    Move(0, 0, 0);
    // 重力を止める
    gravFlag = false;

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
        // 地面いたら上に切れる　空中なら左右のみ
        isGround ? AttackMove(-ax, ay, 30) : AttackMove(-ax, 0, 30);
    }

    // フィニッシャーへの移行
    if (finish) TransitionFinisherState();
    
    // アニメーションが終わった最後の処理
    if (!model->IsPlayAnimatimon()) {
        // 終わったらアイドル状態へ
        TransitionIdleState();
        first = false;
        // 重力を再びオン
        gravFlag = true;

        AttackMove(0, 0, 30);

        velocity = {0,0,0};

        atkPos = { 0,0,0 };
        atk = false;
       
        // ヒットストップおわり
        hitstop = false;

        // カメラシェイク（簡素）おわり
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);
    }
}

void Player::TransitionFinisherState() {
    // 遷移
    state = AnimeState::Finisher;

    model->PlayAnimation(static_cast<int>(state), false);
}

void Player::UpdateFinisherState(float elapsedTime) {
    // アニメーションが終わった最後の処理
    if (!model->IsPlayAnimatimon()) {
        // 終わったらアイドル状態へ
        TransitionIdleState();
        // ヒットストップおわり
        hitstop = false;
        // フィニッシャー終わり
        finish = false;
    }
}

void Player::OnLanding() {
    jumpCount = 0;
}

// 弾丸と敵の衝突判定
void Player::CollisionPanchiVsEnemies() {
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i) {
        Enemy* enemy = enemyManager.GetEnemy(i);
        // 衝突処理
        if (Collision::SphereVsSphere(enemy->GetPosition(), atkPos + position + waistPos, enemy->GetRadius(), atkRadius)) {        
            if (enemy->GetHealth() > 0) {
                enemy->ApplyDamage(1, 0);
                // ヒットストップ
                if (!slow)hitstop = true;

                // カメラシェイク（簡素）
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
        // 衝突処理
        if (Collision::SphereVsSphere(enemy->GetPosition(), atkPos + position + waistPos, enemy->GetRadius(), atkRadius)) {
            enemy->SetReflectionFlag(true);
            enemy->SetDirection(-enemy->GetDirection());
            // ヒットストップ
            if (!slow)hitstop = true;
            // カメラシェイク（簡素）
            CameraManager& cameraMgr = CameraManager::Instance();
            cameraMgr.SetShakeFlag(true);
        }
    }
}

// 敵とSB
void Player::CollisionSBVsEnemies() {
    // 敵探索
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i) {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // SB探索
        SBManager& sbManager = SBManager::Instance();
        int enemyBCount = sbManager.GetProjectileCount();
        for (int i = 0; i < enemyBCount; ++i) {
            SB* sb = sbManager.GetProjectile(i);

            // 衝突処理
            if (Collision::SphereVsSphere(enemy->GetPosition(), sb->GetPosition(), enemy->GetRadius(), atkRadius)) {

                if (enemy->GetHealth() > 0) {
                    enemy->ApplyDamage(1, 0);
                    // ヒットストップ
                    if (!slow) hitstop = true;
                }
                // フィニッシャー発動
                finish = true;
                // 自分を敵の近くへ
                // 自機と敵の位置から左右判定　のちそこから一定距離にワープ　そして殺す
                Vec3 dir = VecMath::Normalize(VecMath::Subtract(position, enemy->GetPosition()));
                dir *= backDir;
                position = enemy->GetPosition() + dir;
                
                /// **********************************
                /// ++++++++++++++++++++++++++++++++++
                ///          回転を反映させる
                /// ++++++++++++++++++++++++++++++++++
                /// **********************************

                // 武器を壊す
                sb->Destroy();
                // 武器を手持ちに
                weapon = true;
            }
        }
    }
}

void Player::CollisionSBVsStage() {
    // SB探索
    SBManager& sbManager = SBManager::Instance();
    int sbCount = sbManager.GetProjectileCount();
    for (int i = 0; i < sbCount; ++i) {
        // SB情報
        SB* sb = sbManager.GetProjectile(i);
        Vec3 pos = sb->GetPosition();
        Vec3 dir = sb->GetDirection();
        float speed = sb->GetSpeed();

        HitResult hit;
        // ステージとの判定
        if (StageManager::Instance().RayCast(pos, pos + VecMath::Normalize(dir) * speed, hit)) {
            // 地面に接地している
            position.x = hit.position.x;
            position.y = hit.position.y;
            position.z = hit.position.z;  
            // 投げた武器の場所にワープ
            position = sb->GetPosition();
            sb->Destroy();
            // 武器を手に持つ
            weapon = true;
        }
    }
}