#include "Player.h"
#include "Framework.h"
#include "Input/Input.h"
#include "Camera/CameraManager.h"

#include "StageManager.h"
#include "DoorManager.h"

#include "EnemyManager.h"
#include "EnemyBulletManager.h"

#include "SBManager.h"
#include "SBNormal.h"

Player::Player(ID3D11Device* device) {
    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();

    const char* idle = "Data/Models/Player/Animations/Idle.fbx";
    const char* run = "Data/Models/Player/Animations/Run.fbx";
    const char* jump = "Data/Models/Player/Animations/Jump.fbx";
    const char* attack = "Data/Models/Player/Animations/Attack.fbx";

    model = new Model(device, "Data/Models/Player/Player.fbx", true, 0);

    model->LoadAnimation(idle, 0, static_cast<int>(AnimeState::Idle));
    model->LoadAnimation(run, 0, static_cast<int>(AnimeState::Run));
    model->LoadAnimation(jump, 0, static_cast<int>(AnimeState::Jump));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Throw));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Attack));
    model->LoadAnimation(attack, 0, static_cast<int>(AnimeState::Finisher));
    model->LoadAnimation(idle, 0, static_cast<int>(AnimeState::Death));

    position = { 0.0f, 0.0f, 0.0f };
    scale = { 0.05f, 0.05f, 0.05f };
    UpdateState[static_cast<int>(AnimeState::Idle)] = &Player::UpdateIdleState;
    UpdateState[static_cast<int>(AnimeState::Run)] = &Player::UpdateRunState;
    UpdateState[static_cast<int>(AnimeState::Jump)] = &Player::UpdateJumpState;
    UpdateState[static_cast<int>(AnimeState::Attack)] = &Player::UpdateAttackState;
    UpdateState[static_cast<int>(AnimeState::Throw)] = &Player::UpdateSBThrowState;
    UpdateState[static_cast<int>(AnimeState::SB)] = &Player::UpdateSBState;
    UpdateState[static_cast<int>(AnimeState::Finisher)] = &Player::UpdateFinisherState;
    UpdateState[static_cast<int>(AnimeState::Death)] = &Player::UpdateDeathState;

    TransitionIdleState();

    HRESULT hr = destructionCb.initialize(device, dc);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
   
    // effect
    hitEffect = new Effect("Data/Effect/playerHit.efk");

    debugRenderer = std::make_unique<DebugRenderer>(device);

    AfterimageManager::Instance().Initialise();
}

Player::~Player() {
    delete hitEffect;
    delete model;
    AfterimageManager::Instance().Destroy();
}

void Player::Init() {
    TransitionIdleState();
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
    // 向き
    direction = -1;
    angle.y = DirectX::XMConvertToRadians(90) * direction;
    // 武器を持つ
    weapon = true;
    // 判定用　半径
    radius = 2.0f;
    // 判定用 体の位置
    waistPos = { 0,3,0 };
    swordPos = { 0,0,0 };
    headPos = { 0,7.5f,0 };
    atkPos = { 0,100,0 };    // 攻撃の位置は攻撃時に設定
    sbLaunchPos = { 0,0,0 };
    // 攻撃
    atk = false;
    atkRadius = 3.0f;
    atkTimer = 0.0f;
    atkPower = 8.8f;

    // ジャンプ関連
    jumpSpeed = 105.0f;
    jumpCount = 0;

    // スローモーション関連    
    playbackSpeed = 1.0f;
    slowSpeed = 0.5f;
    slowAlpha = 0.0f;

    // ヒットストップ
    hitstop = false;
    hitstopSpeed = 0.6f;

    moveVecX = 0.0f;
    moveVecZ = 0.0f;

    height = 8.0f;

    isDead = false;
    reset = false;

    // SB用
    sbSpeed = 5.0f;
    sbSpace = 5.0f;
    sbhit = false;
    sbdir = { 0,0,0 };
    sbPos = { 0,0,0 };
    sbEraseLen = 3000.0f;// SBが消える距離（Sq）60
    sbHitEmy = -1;
    invincible = false;
    blurPower = 0.0f;
    blur = 90.0f;
    stopTime = 0.2f;
    stopTimer = 0.0f;

    slow = false;

    dest.destruction = 0.0f;
    dest.positionFactor = 1.0f;
    dest.rotationFactor = 0.2f;
    dest.scaleFactor = 0.2f;

    health = 1;
    oldHealth = 0;

    // コスト初期化
    if (isTutorial) {
        cost.SetTutorial(isTutorial);
    }
    cost.Reset();

    // 操作可能か
    isControl = true;
    canSlow = true;
    canAttack = true;
    slowFixation = false;
    isTutorial = false;
        
    gravFlag = true;
    gravity = -3.0f;

    // 中心座標更新
    UpdateCenterPosition();

    // 地面貫通するか否か
    penetrate = false;

    deathse = false;
    slowse = false;

    isGround = false;
    isHit = false;
}
#include <Xinput.h>
void Player::Update(float elapsedTime) {
    ID3D11Device* device = Graphics::Ins().GetDevice();
    // スロー中なら、スピードをちょっと上げる
    float modelTime = elapsedTime;
    if (slow && (state == AnimeState::Attack || state == AnimeState::Finisher)) 
        modelTime = elapsedTime / slowSpeed;
    //if (slow) elapsedTime *= 1.2f;

    atkTimer -= elapsedTime;

    /////////////////////////////////////////////////////////
    //                                                   　//
    //                    落ちたら死ぬぞ                    //
    //                                                   　//
    /////////////////////////////////////////////////////////
    FallIsDead();



    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);


    // 残像マネージャー更新
    AfterimageManager::Instance().SetParentData(
        model->GetSkinnedMeshs()->GetMeshs(),
        model->GetSkinnedMeshs()->GetMaterial(),
        model->GetSkinnedMeshs()->GetCb(),
        transform
    );
    AfterimageManager::Instance().Update(elapsedTime);


    UpdateSpeed(elapsedTime);

    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);

    // SB時間制限
    SBManagement(elapsedTime);

    // コスト更新処理
    // シフトブレイク中ならコストを増やさない
    cost.Update(elapsedTime, !(state == AnimeState::SB || state == AnimeState::Throw || state == AnimeState::Finisher));


    // 中心座標更新
    UpdateCenterPosition();

    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetAxisLY() < 0.0f && std::abs(gamePad.GetAxisLY()) >= std::abs(gamePad.GetAxisLX())) {
        penetrate = true;
    }
    else {
        penetrate = false;
    }
    // 旋回処理
    // 移動方向へ向く
    if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;


    CollisionSBVsStage();
    CollisionSBVsEnemies();
    if (atk) CollisionPanchiVsEnemies();
    if (atk) CollisionPanchiVsProjectile();
    
    // SE
    if (atk) {                             
        SEAttack = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Playerattack.wav", false);
        SEAttack.get()->Play(0.5f);
    }

    //オブジェクト行列更新
    UpdateTransform();
    // モデルアニメーション更新処理
    model->UpdateAnimation(modelTime);
    //モデル行列更新
    model->UpdateTransform(transform);

    // 手の位置
    SkinnedMesh::Animation::Keyframe::Node* left = model->FindNode("joint45");
    DirectX::XMFLOAT4X4 p = left->globalTransform;
    DirectX::XMFLOAT4X4 p2;
    using namespace DirectX;
    XMStoreFloat4x4(
        &p2,
        XMLoadFloat4x4(&left->globalTransform) * XMLoadFloat4x4(&transform)
    );
    swordPos = { p2._41, p2._42 - 1.5f, p2._43 };

    // 手の位置
    SkinnedMesh::Animation::Keyframe::Node* sholder = model->FindNode("joint42");
    p = sholder->globalTransform;
    using namespace DirectX;
    XMStoreFloat4x4(
        &p2,
        XMLoadFloat4x4(&sholder->globalTransform) * XMLoadFloat4x4(&transform)
    );
    sbLaunchPos = { p2._41, p2._42, p2._43 };
   
    // 死んだら
    if (health <= 0)
    {
        if (deathse == false)
        {
            SEDeath = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Dead候補1.wav", false);
            //SEDeath = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Dead候補2.wav", false);
            //SEDeath = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Dead候補3.wav", false);
            SEDeath.get()->Play(0.5f);
            deathse = true;
        }
        isDead = true;
    }
    // 生きてるときに当たったら
    if (isHit && !isDead)
    {
        handle = hitEffect->Play(centerPosition, 1.0f);
        isHit = false;
    }


    //position.x = max(position.x, -200);
}


void Player::Render(ID3D11DeviceContext* dc) {
    destructionCb.data.destruction = dest.destruction;
    destructionCb.data.positionFactor = dest.positionFactor;
    destructionCb.data.rotationFactor = dest.rotationFactor;
    destructionCb.data.scaleFactor = dest.scaleFactor;
    destructionCb.applyChanges();
    dc->VSSetConstantBuffers(9, 1, destructionCb.GetAddressOf());
    dc->PSSetConstantBuffers(9, 1, destructionCb.GetAddressOf());
    dc->GSSetConstantBuffers(9, 1, destructionCb.GetAddressOf());


    if (slowAlpha > 0.0f) {
        model->Begin(dc, Shaders::Ins()->GetAfterimage());
        AfterimageManager::Instance().Render(dc);

        if (!isDead) {
            model->Begin(dc, Shaders::Ins()->GetOutline());
            RenderStateKARI::SetCullMode(RenderStateKARI::CU_BACK);
            model->Render(dc);
        }
    }
    model->Begin(dc, Shaders::Ins()->GetDestructionShader());
    model->Render(dc);

    // 弾丸描画処理
    SBManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());

#ifdef _DEBUG
    // height
    Vec3 heightPos = position;
    heightPos.y += height;
    debugRenderer.get()->DrawSphere(heightPos, 1, Vec4(0.5f, 1, 0, 1));
    debugRenderer.get()->DrawSphere(centerPosition, radius, Vec4(0.5f, 1, 0.5f, 1));

    debugRenderer.get()->DrawSphere(position, 1, Vec4(1, 0, 0, 1));

    // 剣根本
    if (atk) debugRenderer.get()->DrawSphere(swordPos, atkRadius, Vec4(1, 0, 0, 1));

    
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
        ImGui::InputInt("JumpCount", &jumpCount);        
        ImGui::RadioButton("IsGround", isGround);

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Position X", &position.x, -300, 300);
            ImGui::SliderFloat("Position Y", &position.y, -200, 200);
            ImGui::SliderFloat("Position Z", &position.z, -300, 300);

            ImGui::SliderFloat("atkPower", &atkPower, 0.0f, 20.0f);

            ImGui::SliderFloat("blurPower", &blurPower, 0,150);
            ImGui::SliderFloat("Height", &height, 0, 10.0f);

            ImGui::SliderFloat("maxMove", &moveSpeed, 0, 100.0f);
            ImGui::SliderFloat("slowSpeed", &slowSpeed, 0, 1.0f);
        }


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
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (deathFlag) return false;

    // 操作不可ならリターン
    if (!isControl)return false;

    // 進行ベクトル所得
    Vec3 moveVec = GetMoveVec();

    // 移動処理
    Move(moveVec.x, moveVec.z, moveSpeed);
    if ((gamePad.GetButtonDown() & GamePad::BTN_RIGHT || gamePad.GetButtonDown() & GamePad::BTN_LEFT) && isGround) {
        SEMove = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Movestart2.wav", false);
        SEMove.get()->Play(0.5f);
    }
    if ((gamePad.GetButtonDown() & GamePad::BTN_AA || gamePad.GetButtonDown() & GamePad::BTN_DD)&&isGround) {
        SEMove = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Movestart2.wav", false);
        SEMove.get()->Play(0.5f);
    }

    // 進行ベクトルがゼロベクトルでない場合は入力された
    return moveVec.x || moveVec.y || moveVec.z;
}

// ジャンプ入力処理
bool Player::InputJump() {
    // 操作不可ならリターン
    if (!isControl)return false;

    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        
        // 空中のときは1回だけ
        if (!isGround)jumpCount += 2;
        // 地面のときは2回
        else jumpCount += 1;
        
        if (jumpCount <= jumpLimit) {
            SEJump = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Jump3.wav", false);
            SEJump.get()->Play(0.3f);
            Jump(jumpSpeed);

            return true;
        }
    }
    return false;
}

void Player::InputSlow(float elapsedTime) {
    // 操作不可ならリターン
    if(!canSlow) if (!isControl) return;
    GamePad& gamePad = Input::Instance().GetGamePad();
    // 押してる間 且 コストがある間
    if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER
        && cost.Approval(elapsedTime)) {

        if (!slowse) {
            SESlowStart = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Slowstart.wav", false);
            SESlowStart.get()->Play(1.0f);
            slowse = true;
        }
        if (cost.Approval(0.5f)) {
            cost.Trg(true);
            slow = true;
        }
        if (cost.GetTrg()) {
            // アルファ値を1/10秒で最大に
            slowAlpha += elapsedTime * 10;
            slowAlpha = min(0.9875f, slowAlpha);
        }
        // コスト枯渇ディレイ期間中
        else if (!cost.Approval(0.5f)) {
            cost.Trg(false);
            slow = false;
            // アルファ値を1/10秒で最小に
            slowAlpha -= elapsedTime * 10;
            slowAlpha = max(0.0f, slowAlpha);
        }
    }
    else if (gamePad.GetButtonUp() & GamePad::BTN_LEFT_TRIGGER) {
            slowse = false;
    }
    else if (!slowFixation) {
        cost.Trg(false);
        slow = false;
        // アルファ値を1/10秒で最小に
        slowAlpha -= elapsedTime * 10;
        slowAlpha = max(0.0f, slowAlpha);
    }
    // スロー固定にしてるとき
    if (slowFixation) {
        // アルファ値を1/10秒で最大に
        slowAlpha += elapsedTime * 10;
        slowAlpha = min(0.9875f, slowAlpha);
    }
}

bool Player::InputSB() {
    // 操作不可ならリターン
    if (!isControl) return false;

    GamePad& gamePad = Input::Instance().GetGamePad();
    ID3D11Device* device = Graphics::Ins().GetDevice();
    Key& key = Input::Instance().GetKey();
    Mouse& mouse = Input::Instance().GetMouse();

    // 武器を持っている場合 pad
    if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER) {
        if (weapon
            && (gamePad.GetAxisLX() != 0 || gamePad.GetAxisLY() != 0)
            && cost.Approval(sbCost)) {
            // 武器を投げる
            weapon = false;
            // 投げる向きを設定
            sbdir = Vec3(-gamePad.GetAxisLX(), gamePad.GetAxisLY(), 0);
            // 向きを設定
            direction = VecMath::sign(-gamePad.GetAxisLX());
            // 旋回処理
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // コスト
            cost.Consume(sbCost);
            SESBstart = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\SBstart.wav", false);
            SESBstart.get()->Play(0.8f);
            return true;
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
                // ワープ先指定
                sbdir = VecMath::Normalize(sb->GetPosition() - position);
                sbPos = sb->GetPosition();
                sbhit = true;
                sb->Destroy();
                return true;
            }
        }
    }

    // 武器を持っている場合 mouse
    if (mouse.GetButtonDown() & Mouse::BTN_RIGHT) {
        if (weapon
            && cost.Approval(sbCost)) {
            // 武器を投げる
            weapon = false;
            // マウスカーソル座標を取得            
            Vec3 cursor;
            cursor.x = static_cast<float>(mouse.GetPositionX());
            cursor.y = static_cast<float>(mouse.GetPositionY());
            cursor.z = 0.0f;
            // player pos
            ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
            // ビューポート
            D3D11_VIEWPORT viewport;
            UINT numViewports = 1;
            dc->RSGetViewports(&numViewports, &viewport);
            // 変換行列
            DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
            DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
            DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
            // プレイヤーの頭上に表示   
            DirectX::XMFLOAT3 worldPosition = centerPosition;

            DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
            // ワールド座標からスクリーン座標へ変換
            DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
                WorldPosition,
                viewport.TopLeftX,
                viewport.TopLeftY,
                viewport.Width,
                viewport.Height,
                viewport.MinDepth,
                viewport.MaxDepth,
                Projection,
                View,
                World
            );
            // スクリーン座標
            Vec2 screenPosition;
            DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);
            Vec3 playerScreenPos;
            playerScreenPos = { screenPosition.x, screenPosition.y, 0.0f };

            // 向き
            Vec3 atkPos = playerScreenPos - cursor;
            atkPos = VecMath::Normalize(atkPos);

            // 向きを設定
            direction = VecMath::sign(atkPos.x);
            // 旋回処理
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // 投げる向きを設定
            sbdir = atkPos;
            // コスト
            cost.Consume(sbCost);
            SESBstart = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\SBstart.wav", false);
            SESBstart.get()->Play(0.8f);
            return true;
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
                return true;
            }
        }
    }
    return false;
}

bool Player::InputAttack() {

    // 操作不可ならリターン
    if (!canAttack) if (!isControl)return false;

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

            // チュートリアル
            if (isTutorial) atkPos = { 4.9f,1.0,0.0 };

            // 向きを設定
            direction = VecMath::sign(atkPos.x);
            // 旋回処理
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;

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

            // チュートリアル
            if (isTutorial) atkPos = { 4.9f,1.0,0.0 };

            // 向きを設定
            direction = VecMath::sign(atkPos.x);
            // 旋回処理
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;

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
    // スロー
    InputSlow(elapsedTime);
    // 死んだら
    if (isDead) {
        TransitionDeathState();
        return;
    }
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
    // スロー
    InputSlow(elapsedTime);
    // 死んだら
    if (isDead) {
        TransitionDeathState();
        return;
    }
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
    // スロー
    InputSlow(elapsedTime);
    // 死んだら
    if (isDead) {
        TransitionDeathState();
        return;
    }
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
    // スロー
    InputSlow(elapsedTime);
    // 死んだら
    if (isDead) {
        TransitionDeathState();
        return;
    }
    static bool first = false;
    if (!first) {
        // 攻撃の向き指定
        Vec3 atkMove = { velocity.x * atkPower,0,0 };
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

    //if (Ground()) velocity.x = 0;

    // 任意のアニメーション再生区間でのみ衝突判定処理をする
    float animationTime = model->GetCurrentAnimationSeconds();
    atk = animationTime >= 0.05f && animationTime <= 0.15f;
    
    // アニメーションが終わった最後の処理
    if (!model->IsPlayAnimatimon()) {
        // 終わったらアイドル状態へ
        TransitionIdleState();

        // 重力を再びオン
        gravFlag = true;

        // 攻撃位置リセット
        atkPos = { 0,-100,0 };
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
    // スロー
    InputSlow(elapsedTime);
    // 死んだら
    if (isDead) {
        TransitionDeathState();
        return;
    }
    // 投げる途中でアニメーション停止
    float animationTime = model->GetCurrentAnimationSeconds();
    if (!model->GetPlay() && animationTime > 0.10f) {
        model->AnimationStop(true);
        Launch(sbdir);
    }
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
    // 無敵
    invincible = true;
    blurPower = 3.0f;
}
void Player::UpdateSBState(float elapsedTime) {
    // スロー
    InputSlow(elapsedTime);
    // 死んだら
    if (isDead) {     
        TransitionDeathState();
        return;
    }
    // ブラー
    blurPower += elapsedTime * blur;
    blurPower = min(blurPower, blurMax);

    // 敵と一定距離近づいていたら
    float len = VecMath::LengthVec3(sbPos - position);
    if (len <= sbSpace) {
        // そのままフィニッシャーステートへ
        sbPos = { 0,0,0 };
        sbdir = { 0,0,0 };
        TransitionFinisherState();
    }
    // 移動後のほうが離れてる = 通り過ぎた
    else {
        // 移動＋レイキャスト
        if (Raycast(sbdir * sbSpeed)) {
            sbPos = { 0,0,0 };
            sbdir = { 0,0,0 };
            TransitionFinisherState();
        }

        // 弾に到達したらSB攻撃ステートへ
        if (VecMath::LengthVec3(sbPos - position) <= sbSpace) {
            position = sbPos;
            sbPos = { 0,0,0 };
            sbdir = { 0,0,0 };
            TransitionFinisherState();
        }
    }
}

void Player::TransitionFinisherState() {
    // 遷移
    state = AnimeState::Finisher;
    // 投げるアニメーション再開
    model->AnimationStop(false);
    // 攻撃の場所
    Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
    atkPos = { front.x,0,0 };
    atkPos = VecMath::Normalize(atkPos) * 3;
    atk = true;
}
void Player::UpdateFinisherState(float elapsedTime) {
    // スロー
    InputSlow(elapsedTime);
    // 死んだら
    if (isDead) {
        TransitionDeathState();
        return;
    }

    // ブラー
    blurPower -= elapsedTime * blur;
    blurPower = max(blurPower, 0.0f);

    // アニメーションが終わったら振り切った姿勢で硬直
    bool stop = false;
    if (!model->IsPlayAnimatimon()) {
        stopTimer += elapsedTime;
        if (stopTimer >= stopTime) stop = true;
    }

    // アニメーションが終わった最後の処理
    if (stop) {
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
        atkPos = { 0,-100,0 };
        atk = false;
        // 無敵解除
        invincible = false;
        // カメラシェイク（簡素）おわり
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);

        // SBが当たった敵を確殺
        if (sbHitEmy >= 0) {
            EnemyManager& enemyManager = EnemyManager::Instance();
            Enemy* enemy = enemyManager.GetEnemy(sbHitEmy);
            // 一定距離近いと殺す
            if(VecMath::LengthVec3(position - enemy->GetPosition()) <= 10)
                enemy->ApplyDamage(1, 0);
            sbHitEmy = -1;
        }
        // ブラーの値
        blurPower = 0.0f;
        // 硬直用タイマー
        stopTimer = 0.0f;
    }
}

void Player::TransitionDeathState() {
    // 遷移
    state = AnimeState::Death;
    // スローに
    slow = true;
    // 重力無くす
    gravFlag = false;
    // 最後のアニメーション状態で止める
    model->AnimationStop(true);
    // 動かさない
    velocity = { 0,0,0 };
}
void Player::UpdateDeathState(float elapsedTime) {
    dest.destruction += elapsedTime * 10.0f;
    // 体が分解しきったら
    if (dest.destruction >= 5.0f) {
        dest.destruction = 5.0f;
        reset = true;
        model->AnimationStop(false);
    }
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
            if (hit.penetrate) {
                // 空中に浮いてる
                position += move;
                isGround = false;
            }
            else {
                // 地面に接地している
                position.y = hit.position.y;
                // 着地した
                if (!isGround) OnLanding();
                isGround = true;
                result = true;
            }
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
        DirectX::XMFLOAT3 start = { position.x, position.y + headPos.y, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + headPos.y + my, position.z };
        // レイキャストによる天井判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            if (hit.penetrate) {
                // 空中に浮いてる
                position += move;
            }
            else {
                // 天井に接している
                position.y = hit.position.y - headPos.y;
                result = true;
            }
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
    float velocityLengthXZ = sqrtf(move.x * move.x);
    // 移動した方向
    direction = VecMath::sign(velocity.x);

    if (velocityLengthXZ > 0.0f) {
        // 水平移動値
        float mx = move.x;

        // レイの開始位置と終点位置(頭)
        DirectX::XMFLOAT3 start2 = { position.x , position.y + stepOffset + waistPos.y, position.z };
        DirectX::XMFLOAT3 end2 = { position.x + mx, position.y + stepOffset + waistPos.y, position.z };

        // レイキャストによる壁判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start2, end2, hit)) {
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
            }
            else {
                position.x = hit2.position.x;
            }
            result = true;
        }
    }
    return result;
}

void Player::SBManagement(float elapsedTime) {
    // 武器を持っていない間
    if (!weapon) {
        SBManager& sbManager = SBManager::Instance();
        int sbCount = sbManager.GetProjectileCount();
        for (int i = 0; i < sbCount; ++i) {
            // SB情報
            SB* sb = sbManager.GetProjectile(i);
            if (VecMath::LengthVec3(sb->GetMoveLen(), true) >= sbEraseLen) {
                weapon = true;
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
    }
}

// 中心座標更新
void Player::UpdateCenterPosition()
{
    centerPosition = position;
    centerPosition.y += height / 2.0f;
}

void Player::Launch(const Vec3& direction) {
    ID3D11Device* device = Graphics::Ins().GetDevice();
    // 発射
    SBNormal* sb = new SBNormal(device, &SBManager::Instance());
    // 向き、　発射地点
    sb->Launch(VecMath::Normalize(direction), sbLaunchPos);
}

// 落下死
void Player::FallIsDead()
{    
    // 生きてるとき
    if (position.y <= -50.0f && !isDead)
    {
        ApplyDamage(10,0);
    }
}

void Player::OnLanding() {
    jumpCount = 0;
}


void Player::CollisionPanchiVsEnemies() {
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i) {
        Enemy* enemy = enemyManager.GetEnemy(i);
        // 衝突処理
        if (Collision::SphereVsSphere(enemy->GetCenterPosition(), swordPos + Vec3(0, -1.5f, 0), enemy->GetRadius(), atkRadius)) {
            if (enemy->GetHealth() > 0) {
                enemy->ApplyDamage(1, 0);
                // ヒットストップ
                if (!slow)hitstop = true;
                // カメラシェイク（簡素）
                CameraManager& cameraMgr = CameraManager::Instance();
                if (!cameraMgr.GetShakeFlag()) {
                    cameraMgr.SetShakeFlag(true);;
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
        if (Collision::SphereVsSphere(enemyBullet->GetPosition(), swordPos, enemyBullet->GetRadius(), atkRadius)) {
            if(!enemyBullet->GetReflectionFlag()) enemyBullet->SetDirection(-enemyBullet->GetDirection());
            enemyBullet->SetReflectionFlag(true);
            // ヒットストップ
            if (!slow)hitstop = true;
            SEReflect = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Reflect.wav", false);
            SEReflect.get()->Play(0.8f);
            // カメラシェイク（簡素）
            CameraManager& cameraMgr = CameraManager::Instance();
            if (!cameraMgr.GetShakeFlag()) {
                cameraMgr.SetShakeFlag(true);
            }

            break;
        }
    }
}
void Player::CollisionSBVsEnemies() {
    // 敵探索
    if (!sbhit) {
        EnemyManager& enemyManager = EnemyManager::Instance();
        int enemyCount = enemyManager.GetEnemyCount();
        for (int i = 0; i < enemyCount; ++i) {
            Enemy* enemy = enemyManager.GetEnemy(i);
            // SB探索
            SBManager& sbManager = SBManager::Instance();
            int enemyBCount = sbManager.GetProjectileCount();
            for (int j = 0; j < enemyBCount; ++j) {
                SB* sb = sbManager.GetProjectile(j);
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
                        // 武器を壊す
                        sb->Destroy();
                        sbHitEmy = i;
                    }
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
            if (hit.penetrate) return;
            // 向きを設定
            direction = VecMath::sign(hit.position.x - position.x);
            // 旋回処理
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // 自分を敵の近くへ
            sbdir = VecMath::Normalize(VecMath::Subtract(hit.position, position));
            // 到達地点
            // 敵の、自分方面に一定距離
            sbPos = hit.position;
            // 弾消す
            sb->Destroy();
            // stageとSBヒット
            sbhit = true;
        }
        else if (DoorManager::Instance().RayCast(pos, pos + VecMath::Normalize(dir) * speed, hit)) {
            // 向きを設定
            direction = VecMath::sign(hit.position.x - position.x);
            // 旋回処理
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // 自分を敵の近くへ
            sbdir = VecMath::Normalize(VecMath::Subtract(hit.position, position));
            // 到達地点
            // 敵の、自分方面に一定距離
            sbPos = hit.position;
            // 弾消す
            sb->Destroy();
            // stageとSBヒット
            sbhit = true;
        }
    }
}