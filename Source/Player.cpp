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
    // 武器を持つ
    weapon = true;
    // 判定用 体の位置
    waistPos = { 0,3,0 };
    headPos = { 0,6,0 };
    atkPos = { 0,0,0 };    // 攻撃の位置は攻撃時に設定
    // 攻撃
    atkRadius = 2;
    atkTimer = 0.0f;
    atkPower = 5.0f;

    // ジャンプ関連
    jumpSpeed = 110.0f;
    jumpCount = 0;

    // スローモーション関連
    playbackSpeed = 1.0f;
    slowSpeed = 0.5f;
    slowTimer = slowMax;
    slowCTTimer = CTMax;

    // ヒットストップ
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

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);
   
    // スロー
    InputSlow(elapsedTime);

    // 旋回処理
    // 移動方向へ向く
    if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;


    CollisionSBVsEnemies();
    CollisionSBVsStage();
    if (atk) CollisionPanchiVsEnemies();
    if (atk) CollisionPanchiVsProjectile();

    atkTimer -= elapsedTime;

    Vibration(elapsedTime);

    //オブジェクト行列更新
    UpdateTransform();
    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);
    //モデル行列更新
    model->UpdateTransform(transform);



    // 死んだら
    if (health <= 0)isDead = true;
}


void Player::Render(ID3D11DeviceContext* dc) {
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
    model->Render(dc);

    // 弾丸描画処理
    SBManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());

    centerPosition = position;
    centerPosition.y += height / 2.0f;

#ifdef _DEBUG
    // height
    Vec3 heightPos = position;
    heightPos.y += height;
    debugRenderer.get()->DrawSphere(heightPos, 1, Vec4(0.5f, 1, 0, 1));

    // 必要なったら追加
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
        // トランスフォーム
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

    // 進行ベクトルがゼロベクトルでない場合は入力された
    return moveVec.x || moveVec.y || moveVec.z;
}

// ジャンプ入力処理
bool Player::InputJump() {
    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
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
    // 押してる間
    if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER) {
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
    if (!(gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER) && slowTimer < slowMax && slowCT == 0) {
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
}

bool Player::InputSB() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    ID3D11Device* device = Graphics::Ins().GetDevice();
    Key& key = Input::Instance().GetKey();
    Mouse& mouse = Input::Instance().GetMouse();

    // 武器を持っている場合 pad
    if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER) {
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
                // 向きを設定
                direction = VecMath::sign(sb->GetPosition().x - position.x);
                // 旋回処理
                if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                // ワープ先指定
                sbdir = VecMath::Normalize(sb->GetPosition() - position);
                sbPos = sb->GetPosition();
                sbhit = true;
                sb->Destroy();
            }
        }
        return true;
    }

    // 武器を持っている場合 mouse
    if (mouse.GetButtonDown() & Mouse::BTN_RIGHT) {
        if (weapon) {
            // 武器を投げる
            weapon = false;
            // 発射
            SBNormal* sb = new SBNormal(device, &SBManager::Instance());

            // マウスカーソル座標を取得            
            Vec3 cursor;
            cursor.x = static_cast<float>(mouse.GetPositionX());
            cursor.y = static_cast<float>(mouse.GetPositionY());
            cursor.z = 0.0f;
            // player pos
            Vec3 playerScreenPos;
            playerScreenPos = { Graphics::Ins().GetScreenWidth() * 0.5f, Graphics::Ins().GetScreenHeight() * 0.5f + 3.0f, 0.0f };

            // 向き
            Vec3 atkPos = playerScreenPos - cursor;
            atkPos = VecMath::Normalize(atkPos);            

            // 発射地点
            sb->Launch(atkPos, position + waistPos);
        }
        // 武器を持っていない
        else if (!weapon) {
            // SB探索
            SBManager& sbManager = SBManager::Instance();
            int sbCount = sbManager.GetProjectileCount();
            for (int i = 0; i < sbCount; ++i) {
                SB* sb = sbManager.GetProjectile(i);

                // 向きを設定
                direction = VecMath::sign(sb->GetPosition().x - position.x);
                // 旋回処理
                if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                // 投げた武器の場所にワープ
                // ワープ先指定
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
    // 入力情報を所得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();
    Mouse& mouse = Input::Instance().GetMouse();

    // 武器を持っている　
    if (weapon && atkTimer < 0.0f) {
        // 攻撃
        if (gamePad.GetButtonDown() & GamePad::BTN_X) {
            // 攻撃の場所
            Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
            if (ax && ay)atkPos = { -ax, ay, 0 };
            else atkPos = { front.x,0,0 };
            atkPos = VecMath::Normalize(atkPos) * 3;

            // 攻撃のCT
            if(!isGround) atkTimer = 0.50f;
            return true;
        }
        else if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
            
            // マウスカーソル座標を取得            
            Vec3 cursor;                    
            cursor.x = static_cast<float>(mouse.GetPositionX());
            cursor.y = static_cast<float>(mouse.GetPositionY());
            cursor.z = 0.0f;
            
            Vec3 playerScreenPos;
            playerScreenPos = { Graphics::Ins().GetScreenWidth() * 0.5f, Graphics::Ins().GetScreenHeight() * 0.5f + 3.0f, 0.0f };
            
            atkPos = playerScreenPos - cursor;
            atkPos = VecMath::Normalize(atkPos) * 5;

            // 攻撃のCT
            if (!isGround) atkTimer = 0.50f;
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
    // ジャンプ入力処理
    if (InputJump()) TransitionJumpState();
    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();
    // シフトブレイク
    if (InputSB()) TransitionSBThrowState();
}

//走るステート遷移
void Player::TransitionRunState() {
    state = AnimeState::Run;
    model->PlayAnimation(static_cast<int>(state), true);
}
//走るステート更新処理
void Player::UpdateRunState(float elapsedTime) {
    //  移動入力処理
    if (!InputMove(elapsedTime)) TransitionIdleState();
    // 攻撃入力処理
    if (InputAttack()) TransitionAttackState();
    // ジャンプ入力処理
    if (InputJump()) TransitionJumpState();
    // シフトブレイク
    if (InputSB()) TransitionSBThrowState();
}

//ジャンプステート遷移
void Player::TransitionJumpState() {
    state = AnimeState::Jump;
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
    // シフトブレイク
    if (InputSB()) TransitionSBThrowState();
}

void Player::TransitionAttackState() {
    state = AnimeState::Attack;
    model->PlayAnimation(static_cast<int>(state), false);
    //移動力を攻撃に乗せない
    velocity = {0,0,0};
}
void Player::UpdateAttackState(float elapsedTime) {
    static bool first = false;
    if (!first) {
        // 攻撃の向き指定
        Vec3 atkMove = atkPos * atkPower;
        if (!Ground()) {
            // 空中にいるときの攻撃は上に飛ばない
            atkMove.y = 0;
            // 空中攻撃の重力は軽く滞空させる
            gravity = -0.0f;
        }
        else {
            // 地面から空中に向けて攻撃してるとき
            if (atkMove.y > 0) {
                // 摩擦を弱めるために空中にする
                position.y += 0.01f;
                isGround = false;
            }
        }
        // 攻撃方向へとばす
        AddImpulse(atkMove);
        first = true;
    }

    if (Ground()) velocity.x = 0;

    // 任意のアニメーション再生区間でのみ衝突判定処理をする
    float animationTime = model->GetCurrentAnimationSeconds();
    atk = animationTime >= 0.01f && animationTime <= 0.20f;
    
    // アニメーションが終わった最後の処理
    if (!model->IsPlayAnimatimon()) {
        // 終わったらアイドル状態へ
        TransitionIdleState();

        // 重力を再びオン
        gravFlag = true;

        // 攻撃位置リセット
        atkPos = { 0,0,0 };
        atk = false;
       
        // ヒットストップおわり
        hitstop = false;

        // 重力戻す
        gravity = -3.0f;

        // カメラシェイク（簡素）おわり
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);
        first = false;
    }
}

void Player::TransitionSBThrowState() {
    // 遷移
    state = AnimeState::Throw;

    model->PlayAnimation(static_cast<int>(state), false);
    // SB中重力なし
    gravFlag = false;
    velocity = { 0,0,0 };
}
void Player::UpdateSBThrowState(float elapsedTime) {
    // SB発動したら次のステートへ
    if (InputSB()) TransitionSBState();
    // 何かに当たったら発動
    if(sbhit) TransitionSBState();
}

void Player::TransitionSBState() {
    // 遷移
    state = AnimeState::SB;
    sbhit = false;
    clock = true;
    // アニメーションは止める
    //model->PlayAnimation(static_cast<int>(state), false);
}
void Player::UpdateSBState(float elapsedTime) {
    // 移動＋レイキャスト
    if(Raycast(sbdir * sbSpeed)) {
        sbPos = { 0,0,0 };
        TransitionFinisherState();
    }

    //position += sbdir * sbSpeed;

    // 敵に到達したらSB攻撃ステートへ
    if (VecMath::LengthVec3(sbPos - position) <= sbSpace) {
        position = sbPos;
        sbPos = { 0,0,0 };
        TransitionFinisherState();
    }
}

void Player::TransitionFinisherState() {
    // 遷移
    state = AnimeState::Finisher;
    model->PlayAnimation(static_cast<int>(state), false);
    // ヒットストップ
    if (!slow) hitstop = true;
    // カメラシェイク（簡素）
    CameraManager& cameraMgr = CameraManager::Instance();
    if (!cameraMgr.GetShakeFlag()) {
        cameraMgr.SetShakeFlag(true);
        vibration = true;
        vibTimer = 0.4f;
    }

    // 攻撃の場所
    Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
    atkPos = { front.x,0,0 };
    atkPos = VecMath::Normalize(atkPos) * 3;
}
void Player::UpdateFinisherState(float elapsedTime) {
    // 任意のアニメーション再生区間でのみ衝突判定処理をする
    float animationTime = model->GetCurrentAnimationSeconds();
    atk = animationTime >= 0.01f && animationTime <= 0.20f;

    // アニメーションが終わった最後の処理
    if (!model->IsPlayAnimatimon()) {
        // 終わったらアイドル状態へ
        TransitionIdleState();
        // ヒットストップおわり
        hitstop = false;
        // 武器を手に持つ
        weapon = true;
        //重力復活
        gravFlag = true;
        // 時間戻す
        clock = false;
        // 攻撃位置リセット
        atkPos = { 0,0,0 };
        atk = false;
        // カメラシェイク（簡素）おわり
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);
    }
}


void Player::Vibration(float elapsedTime) {
    //// 振動試し
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

    // 移動量
    float my = move.y;
    bool result = false;

    // 落下中
    if (my < 0.0f) {
        // レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        // レイキャストによる地面判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {

            // 地面に接地している
            position.x = hit.position.x;
            position.y = hit.position.y;
            position.z = hit.position.z;

            // 回転
            angle.y += hit.rotation.y;

            //  傾斜率の計算
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            // 着地した
            if (!isGround) OnLanding();

            isGround = true;
            velocity.y = 0.0f;
            result = true;
        }
        else {
            // 空中に浮いてる
            position += move;
            isGround = false;
        }
    }
    // 上昇中
    else if (my > 0.0f) {

        // レイの開始位置は頭
        DirectX::XMFLOAT3 start = { position.x, position.y + height, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + height + my, position.z };

        // レイキャストによる天井判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {

            // 天井に接している
            position.x = hit.position.x;
            position.y = hit.position.y - height;
            position.z = hit.position.z;

            // 回転
            angle.y += hit.rotation.y;

            velocity.y = 0.0f;
            result = true;
        }
        else {
            // 空中に浮いてる
            position += move;
        }
    }
    else {
        position += move;
    }



    // 水平速力計算
    float velocityLengthXZ = sqrtf(move.x * move.x + move.z * move.z);
    // 移動した方向
    direction = VecMath::sign(velocity.x);

    if (velocityLengthXZ > 0.0f) {
        // 水平移動値
        float mx = move.x;

        // レイの開始位置と終点位置
        DirectX::XMFLOAT3 start = { position.x , position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z };
        // レイの開始位置と終点位置(頭)
        DirectX::XMFLOAT3 start2 = { position.x , position.y + stepOffset + height, position.z };
        DirectX::XMFLOAT3 end2 = { position.x + mx, position.y + stepOffset + height, position.z };

        // レイキャストによる壁判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影                          // ベクトルの否定する
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // 補正位置の計算                   // v1 ベクトル乗数 v2 ベクター乗算   3 番目のベクトルに追加された最初の 2 つのベクトルの積を計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // 戻り値 ベクトルの積和を返す    戻り値　＝　v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // 補正位置が壁に埋まっているかどうか
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
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start2);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end2);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影                          // ベクトルの否定する
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // 補正位置の計算                   // v1 ベクトル乗数 v2 ベクター乗算   3 番目のベクトルに追加された最初の 2 つのベクトルの積を計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // 戻り値 ベクトルの積和を返す    戻り値　＝　v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // 補正位置が壁に埋まっているかどうか
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
        // 衝突処理
        if (Collision::SphereVsSphere(enemyBullet->GetPosition(), atkPos + position + waistPos, enemyBullet->GetRadius(), atkRadius)) {
            if(!enemyBullet->GetReflectionFlag()) enemyBullet->SetDirection(-enemyBullet->GetDirection());
            enemyBullet->SetReflectionFlag(true);
            // ヒットストップ
            if (!slow)hitstop = true;
            // カメラシェイク（簡素）
            CameraManager& cameraMgr = CameraManager::Instance();
            if (!cameraMgr.GetShakeFlag()) {
                cameraMgr.SetShakeFlag(true);
                vibration = true;
                vibTimer = 0.4f;
            }
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


                    // 向きを設定
                    direction = VecMath::sign(enemy->GetPosition().x - position.x);
                    // 旋回処理
                    if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                    // 敵とSBヒット
                    sbhit = true;

                    // 自分を敵の近くへ
                    // 自機と敵の位置から左右判定　のちそこから一定距離にワープ　そして殺す
                    sbdir = VecMath::Normalize(VecMath::Subtract(enemy->GetPosition(), position));
                    // 到達地点
                    // 敵の、自分方面に一定距離
                    sbPos = enemy->GetPosition();
                    sbPos.x += -(VecMath::sign(enemy->GetPosition().x - position.x)) * sbSpace;

                    //Vec3 dir = VecMath::Normalize(VecMath::Subtract(position, enemy->GetPosition()));
                    //dir *= backDir;
                    //position = enemy->GetPosition() + dir;
                    //enemy->ApplyDamage(1, 0);

                    // 武器を壊す
                    sb->Destroy();
                }
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
            // 向きを設定
            direction = VecMath::sign(hit.position.x - position.x);
            // 旋回処理
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;

            // 自分を敵の近くへ
            sbdir = VecMath::Normalize(VecMath::Subtract(hit.position, position));
            // 到達地点
            // 敵の、自分方面に一定距離
            sbPos = hit.position;


            sb->Destroy();
            // stageとSBヒット
            sbhit = true;
        }
    }
}