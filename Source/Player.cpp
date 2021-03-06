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
    // ????
    direction = -1;
    angle.y = DirectX::XMConvertToRadians(90) * direction;
    // ??????????
    weapon = true;
    // ?????p?@???a
    radius = 2.0f;
    // ?????p ???????u
    waistPos = { 0,3,0 };
    swordPos = { 0,0,0 };
    headPos = { 0,7.5f,0 };
    atkPos = { 0,100,0 };    // ?U???????u???U??????????
    sbLaunchPos = { 0,0,0 };
    // ?U??
    atk = false;
    atkRadius = 4.0f;
    atkTimer = 0.0f;
    atkPower = 7.5f;

    // ?W?????v???A
    jumpSpeed = 105.0f;
    jumpCount = 0;

    // ?X???[???[?V???????A    
    playbackSpeed = 1.0f;
    slowSpeed = 0.5f;
    slowAlpha = 0.0f;

    // ?q?b?g?X?g?b?v
    hitstop = false;
    hitstopSpeed = 0.6f;

    moveVecX = 0.0f;
    moveVecZ = 0.0f;

    height = 8.0f;

    isDead = false;
    reset = false;

    // SB?p
    sbSpeed = 5.0f;
    sbSpace = 5.0f;
    sbhit = false;
    sbdir = { 0,0,0 };
    sbPos = { 0,0,0 };
    sbEraseLen = 3000.0f;// SB?????????????iSq?j60
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

    // ?R?X?g??????
    if (isTutorial) {
        cost.SetTutorial(isTutorial);
    }
    cost.Reset();

    // ???????\??
    isControl = true;
    canSlow = true;
    canAttack = true;
    slowFixation = false;
    isTutorial = false;
        
    gravFlag = true;
    gravity = -3.0f;

    // ???S???W?X?V
    UpdateCenterPosition();

    // ?n????????????????
    penetrate = false;

    deathse = false;
    slowse = false;

    isGround = false;
    isHit = false;
}
#include <Xinput.h>
void Player::Update(float elapsedTime) {
    ID3D11Device* device = Graphics::Ins().GetDevice();
    // ?X???[???????A?X?s?[?h????????????????
    float modelTime = elapsedTime;
    if (slow && (state == AnimeState::Attack || state == AnimeState::Finisher)) 
        modelTime = elapsedTime / slowSpeed;
    //if (slow) elapsedTime *= 1.2f;

    atkTimer -= elapsedTime;

    /////////////////////////////////////////////////////////
    //                                                   ?@//
    //                    ??????????????                    //
    //                                                   ?@//
    /////////////////////////////////////////////////////////
    FallIsDead();



    (this->*UpdateState[static_cast<int>(state)])(elapsedTime);


    // ?c???}?l?[?W???[?X?V
    AfterimageManager::Instance().SetParentData(
        model->GetSkinnedMeshs()->GetMeshs(),
        model->GetSkinnedMeshs()->GetMaterial(),
        model->GetSkinnedMeshs()->GetCb(),
        transform
    );
    AfterimageManager::Instance().Update(elapsedTime);


    UpdateSpeed(elapsedTime);

    // ???G?????X?V
    UpdateInvincibleTimer(elapsedTime);

    // SB????????
    SBManagement(elapsedTime);

    // ?R?X?g?X?V????
    // ?V?t?g?u???C?N???????R?X?g????????????
    cost.Update(elapsedTime, !(state == AnimeState::SB || state == AnimeState::Throw || state == AnimeState::Finisher));


    // ???S???W?X?V
    UpdateCenterPosition();

    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetAxisLY() < 0.0f && std::abs(gamePad.GetAxisLY()) >= std::abs(gamePad.GetAxisLX())) {
        penetrate = true;
    }
    else {
        penetrate = false;
    }
    // ????????
    // ??????????????
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

    //?I?u?W?F?N?g?s???X?V
    UpdateTransform();
    // ???f???A?j???[?V?????X?V????
    model->UpdateAnimation(modelTime);
    //???f???s???X?V
    model->UpdateTransform(transform);

    // ???????u
    SkinnedMesh::Animation::Keyframe::Node* left = model->FindNode("joint45");
    DirectX::XMFLOAT4X4 p = left->globalTransform;
    DirectX::XMFLOAT4X4 p2;
    using namespace DirectX;
    XMStoreFloat4x4(
        &p2,
        XMLoadFloat4x4(&left->globalTransform) * XMLoadFloat4x4(&transform)
    );
    swordPos = { p2._41, p2._42 - 1.5f, p2._43 };

    // ???????u
    SkinnedMesh::Animation::Keyframe::Node* sholder = model->FindNode("joint42");
    p = sholder->globalTransform;
    using namespace DirectX;
    XMStoreFloat4x4(
        &p2,
        XMLoadFloat4x4(&sholder->globalTransform) * XMLoadFloat4x4(&transform)
    );
    sbLaunchPos = { p2._41, p2._42, p2._43 };
   
    // ????????
    if (health <= 0)
    {
        if (deathse == false)
        {
            SEDeath = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Dead????1.wav", false);
            //SEDeath = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Dead????2.wav", false);
            //SEDeath = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Dead????3.wav", false);
            SEDeath.get()->Play(0.5f);
            deathse = true;
        }
        isDead = true;
    }
    // ????????????????????????
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
        if (!isDead) {
            model->Begin(dc, Shaders::Ins()->GetAfterimage());
            AfterimageManager::Instance().Render(dc);
            model->Begin(dc, Shaders::Ins()->GetOutline());
            RenderStateKARI::SetCullMode(RenderStateKARI::CU_BACK);
            model->Render(dc);
        }
    }
    model->Begin(dc, Shaders::Ins()->GetDestructionShader());
    model->Render(dc);

    // ?e???`??????
    SBManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());

#ifdef _DEBUG
    // height
    Vec3 heightPos = position;
    heightPos.y += height;
    debugRenderer.get()->DrawSphere(heightPos, 1, Vec4(0.5f, 1, 0, 1));
    debugRenderer.get()->DrawSphere(centerPosition, radius, Vec4(0.5f, 1, 0.5f, 1));

    debugRenderer.get()->DrawSphere(position, 1, Vec4(1, 0, 0, 1));

    // ?????{
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
        // ?g?????X?t?H?[??
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
    // ??????????????
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // ?J???????????X?e?B?b?N???????l?????????i?s???????v?Z????
    const DirectX::XMFLOAT3& cameraRight = CameraManager::Instance().mainC.GetRight();
    const DirectX::XMFLOAT3& cameraFront = CameraManager::Instance().mainC.GetFront();

    // ?????x?N?g????xz?????????????x?N?g??????????????????
    // ?J?????E?????x?N?g????xz?P???x?N?g????????
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f) {
        // ?P???x?N?g????
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    // ?J?????O?????x?N?g????XZ?P???x?N?g????????
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f) {
        // ?P???x?N?g????
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    // ?X?e?B?b?N???????????l???J?????E?????????f???A
    // ?X?e?B?b?N???????????l???J?????O?????????f???A
    // ?i?s?x?N?g?????v?Z????
    Vec3 vec;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    // Y????????????????????
    vec.y = 0.0f;

    vec.x = -ax;
    vec.y = 0;
    vec.z = 0;

    return vec;
}

bool Player::InputMove(float elapsedTime) {
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (deathFlag) return false;

    // ?????s?????????^?[??
    if (!isControl)return false;

    // ?i?s?x?N?g??????
    Vec3 moveVec = GetMoveVec();

    // ????????
    Move(moveVec.x, moveVec.z, moveSpeed);
    if ((gamePad.GetButtonDown() & GamePad::BTN_RIGHT || gamePad.GetButtonDown() & GamePad::BTN_LEFT) && isGround) {
        SEMove = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Movestart2.wav", false);
        SEMove.get()->Play(0.5f);
    }
    if ((gamePad.GetButtonDown() & GamePad::BTN_AA || gamePad.GetButtonDown() & GamePad::BTN_DD)&&isGround) {
        SEMove = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Movestart2.wav", false);
        SEMove.get()->Play(0.5f);
    }

    // ?i?s?x?N?g?????[???x?N?g????????????????????????
    return moveVec.x || moveVec.y || moveVec.z;
}

// ?W?????v????????
bool Player::InputJump() {
    // ?????s?????????^?[??
    if (!isControl)return false;

    Key& key = Input::Instance().GetKey();
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        
        // ????????????1??????
        if (!isGround)jumpCount += 2;
        // ?n??????????2??
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
    // ?????s?????????^?[??
    if(!canSlow) if (!isControl) return;
    GamePad& gamePad = Input::Instance().GetGamePad();
    // ?????????? ?? ?R?X?g????????
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
            // ?A???t?@?l??1/10?b????????
            slowAlpha += elapsedTime * 10;
            slowAlpha = min(0.98f, slowAlpha);
        }
        // ?R?X?g?????f?B???C??????
        else if (!cost.Approval(0.5f)) {
            cost.Trg(false);
            slow = false;
            // ?A???t?@?l??1/10?b????????
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
        // ?A???t?@?l??1/10?b????????
        slowAlpha -= elapsedTime * 10;
        slowAlpha = max(0.0f, slowAlpha);
    }
    // ?X???[????????????????
    if (slowFixation) {
        // ?A???t?@?l??1/10?b????????
        slowAlpha += elapsedTime * 10;
        slowAlpha = min(0.98f, slowAlpha);
    }
}

bool Player::InputSB() {
    // ?????s?????????^?[??
    if (!isControl) return false;

    GamePad& gamePad = Input::Instance().GetGamePad();
    ID3D11Device* device = Graphics::Ins().GetDevice();
    Key& key = Input::Instance().GetKey();
    Mouse& mouse = Input::Instance().GetMouse();

    // ???????????????????? pad
    if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER) {
        if (weapon
            && (gamePad.GetAxisLX() != 0 || gamePad.GetAxisLY() != 0)
            && cost.Approval(sbCost)) {
            // ????????????
            weapon = false;
            // ????????????????
            sbdir = Vec3(-gamePad.GetAxisLX(), gamePad.GetAxisLY(), 0);
            // ??????????
            direction = VecMath::sign(-gamePad.GetAxisLX());
            // ????????
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // ?R?X?g
            cost.Consume(sbCost);
            SESBstart = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\SBstart.wav", false);
            SESBstart.get()->Play(0.8f);
            return true;
        }
        // ??????????????????
        else if (!weapon) {
            // SB?T??
            SBManager& sbManager = SBManager::Instance();
            int sbCount = sbManager.GetProjectileCount();
            for (int i = 0; i < sbCount; ++i) {
                SB* sb = sbManager.GetProjectile(i);
                // ??????????
                direction = VecMath::sign(sb->GetPosition().x - position.x);
                // ????????
                if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                // ???[?v???w??
                sbdir = VecMath::Normalize(sb->GetPosition() - position);
                sbPos = sb->GetPosition();
                sbhit = true;
                sb->Destroy();
                return true;
            }
        }
    }

    // ???????????????????? mouse
    if (mouse.GetButtonDown() & Mouse::BTN_RIGHT) {
        if (weapon
            && cost.Approval(sbCost)) {
            // ????????????
            weapon = false;
            // ?}?E?X?J?[?\?????W??????            
            Vec3 cursor;
            cursor.x = static_cast<float>(mouse.GetPositionX());
            cursor.y = static_cast<float>(mouse.GetPositionY());
            cursor.z = 0.0f;
            // player pos
            ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
            // ?r???[?|?[?g
            D3D11_VIEWPORT viewport;
            UINT numViewports = 1;
            dc->RSGetViewports(&numViewports, &viewport);
            // ?????s??
            DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
            DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
            DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
            // ?v???C???[?????????\??   
            DirectX::XMFLOAT3 worldPosition = centerPosition;

            DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
            // ???[???h???W?????X?N???[?????W??????
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
            // ?X?N???[?????W
            Vec2 screenPosition;
            DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);
            Vec3 playerScreenPos;
            playerScreenPos = { screenPosition.x, screenPosition.y, 0.0f };

            // ????
            Vec3 atkPos = playerScreenPos - cursor;
            atkPos = VecMath::Normalize(atkPos);

            // ??????????
            direction = VecMath::sign(atkPos.x);
            // ????????
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // ????????????????
            sbdir = atkPos;
            // ?R?X?g
            cost.Consume(sbCost);
            SESBstart = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\SBstart.wav", false);
            SESBstart.get()->Play(0.8f);
            return true;
        }
        // ??????????????????
        else if (!weapon) {
            // SB?T??
            SBManager& sbManager = SBManager::Instance();
            int sbCount = sbManager.GetProjectileCount();
            for (int i = 0; i < sbCount; ++i) {
                SB* sb = sbManager.GetProjectile(i);

                // ??????????
                direction = VecMath::sign(sb->GetPosition().x - position.x);
                // ????????
                if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                // ?????????????????????[?v
                // ???[?v???w??
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

    // ?????s?????????^?[??
    if (!canAttack) if (!isControl)return false;

    // ??????????????
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();
    Mouse& mouse = Input::Instance().GetMouse();

    // ?????????????????@
    if (weapon && atkTimer < 0.0f) {
        // ?U??
        if (gamePad.GetButtonDown() & GamePad::BTN_X) {
            // ?U????????
            Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
            if (ax && ay)atkPos = { -ax, ay, 0 };
            else atkPos = { front.x,0,0 };
            atkPos = VecMath::Normalize(atkPos) * 3;

            // ?`???[?g???A??
            if (isTutorial) atkPos = { 4.9f,1.0,0.0 };

            // ??????????
            direction = VecMath::sign(atkPos.x);
            // ????????
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;

            // ?U????CT
            if(!isGround) atkTimer = 0.50f;
            return true;
        }
        else if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
            
            // ?}?E?X?J?[?\?????W??????            
            Vec3 cursor;                    
            cursor.x = static_cast<float>(mouse.GetPositionX());
            cursor.y = static_cast<float>(mouse.GetPositionY());
            cursor.z = 0.0f;
            
            Vec3 playerScreenPos;
            playerScreenPos = { Graphics::Ins().GetScreenWidth() * 0.5f, Graphics::Ins().GetScreenHeight() * 0.5f + 3.0f, 0.0f };
            
            atkPos = playerScreenPos - cursor;
            atkPos = VecMath::Normalize(atkPos) * 5;

            // ?`???[?g???A??
            if (isTutorial) atkPos = { 4.9f,1.0,0.0 };

            // ??????????
            direction = VecMath::sign(atkPos.x);
            // ????????
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;

            // ?U????CT
            if (!isGround) atkTimer = 0.50f;
            return true;
        }
    }
    return false;
}

// ???@?X?e?[?g?J??
void Player::TransitionIdleState() {
    state = AnimeState::Idle;
    model->PlayAnimation(static_cast<int>(state), true);
}
// ???@?X?e?[?g?X?V????
void Player::UpdateIdleState(float elapsedTime) {
    // ?X???[
    InputSlow(elapsedTime);
    // ????????
    if (isDead) {
        TransitionDeathState();
        return;
    }
    //  ????????????
    if (InputMove(elapsedTime)) TransitionRunState();
    // ?W?????v????????
    if (InputJump()) TransitionJumpState();
    // ?U??????????
    if (InputAttack()) TransitionAttackState();
    // ?V?t?g?u???C?N
    if (InputSB()) TransitionSBThrowState();
}

//?????X?e?[?g?J??
void Player::TransitionRunState() {
    state = AnimeState::Run;
    model->PlayAnimation(static_cast<int>(state), true);
}
//?????X?e?[?g?X?V????
void Player::UpdateRunState(float elapsedTime) {
    // ?X???[
    InputSlow(elapsedTime);
    // ????????
    if (isDead) {
        TransitionDeathState();
        return;
    }
    //  ????????????
    if (!InputMove(elapsedTime)) TransitionIdleState();
    // ?U??????????
    if (InputAttack()) TransitionAttackState();
    // ?W?????v????????
    if (InputJump()) TransitionJumpState();
    // ?V?t?g?u???C?N
    if (InputSB()) TransitionSBThrowState();
}

//?W?????v?X?e?[?g?J??
void Player::TransitionJumpState() {
    state = AnimeState::Jump;
    model->PlayAnimation(static_cast<int>(state), false);
}
//?W?????v?X?e?[?g?X?V????
void Player::UpdateJumpState(float elapsedTime) {
    // ?X???[
    InputSlow(elapsedTime);
    // ????????
    if (isDead) {
        TransitionDeathState();
        return;
    }
    //  ????????????
    InputMove(elapsedTime);
    // ?U??????????
    if (InputAttack()) TransitionAttackState();
    // ?W?????v????????
    if (InputJump()) TransitionJumpState();
    // ?n?????????????A?C?h????????
    if (isGround) TransitionIdleState();
    // ?V?t?g?u???C?N
    if (InputSB()) TransitionSBThrowState();
}

void Player::TransitionAttackState() {
    state = AnimeState::Attack;
    model->PlayAnimation(static_cast<int>(state), false);
    //?????????U????????????
    velocity = {0,0,0};
}
void Player::UpdateAttackState(float elapsedTime) {
    // ?X???[
    InputSlow(elapsedTime);
    // ????????
    if (isDead) {
        TransitionDeathState();
        return;
    }
    static bool first = false;
    if (!first) {
        // ?U?????????w??
        Vec3 atkMove = { velocity.x * atkPower,0,0 };
        if (!Ground()) {
            // ?????????????????U????????????????
            atkMove.y = 0;
            // ?????U?????d?????y????????????
            gravity = -0.0f;
        }
        else {
            // ?n???????????????????U????????????
            if (atkMove.y > 0) {
                // ???C????????????????????????
                position.y += 0.01f;
                isGround = false;
            }
        }
        // ?U??????????????
        AddImpulse(atkMove);
        first = true;
    }

    //if (Ground()) velocity.x = 0;

    // ?C?????A?j???[?V????????????????????????????????????
    float animationTime = model->GetCurrentAnimationSeconds();
    atk = animationTime >= 0.05f && animationTime <= 0.15f;
    
    // ?A?j???[?V???????I????????????????
    if (!model->IsPlayAnimatimon()) {
        // ?I?????????A?C?h????????
        TransitionIdleState();

        // ?d?????????I??
        gravFlag = true;

        // ?U?????u???Z?b?g
        atkPos = { 0,-100,0 };
        atk = false;
       
        // ?q?b?g?X?g?b?v??????
        hitstop = false;

        // ?d??????
        gravity = -3.0f;

        // ?J?????V?F?C?N?i???f?j??????
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);
        first = false;
    }
}

void Player::TransitionSBThrowState() {
    // ?J??
    state = AnimeState::Throw;

    model->PlayAnimation(static_cast<int>(state), false);
    // SB???d??????
    gravFlag = false;
    velocity = { 0,0,0 };
}
void Player::UpdateSBThrowState(float elapsedTime) {
    // ?X???[
    InputSlow(elapsedTime);
    // ????????
    if (isDead) {
        TransitionDeathState();
        return;
    }
    // ???????r?????A?j???[?V???????~
    float animationTime = model->GetCurrentAnimationSeconds();
    if (!model->GetPlay() && animationTime > 0.10f) {
        model->AnimationStop(true);
        Launch(sbdir);
    }
    // SB???????????????X?e?[?g??
    if (InputSB()) TransitionSBState();
    // ????????????????????
    if(sbhit) TransitionSBState();
}

void Player::TransitionSBState() {
    // ?J??
    state = AnimeState::SB;
    sbhit = false;
    clock = true;
    // ???G
    invincible = true;
    blurPower = 3.0f;
}
void Player::UpdateSBState(float elapsedTime) {
    // ?X???[
    InputSlow(elapsedTime);
    // ????????
    if (isDead) {     
        TransitionDeathState();
        return;
    }
    // ?u???[
    blurPower += elapsedTime * blur;
    blurPower = min(blurPower, blurMax);

    // ?G????????????????????????
    float len = VecMath::LengthVec3(sbPos - position);
    if (len <= sbSpace) {
        // ?????????t?B?j?b?V???[?X?e?[?g??
        sbPos = { 0,0,0 };
        sbdir = { 0,0,0 };
        TransitionFinisherState();
    }
    // ?????????????????????? = ??????????
    else {
        // ?????{???C?L???X?g
        if (Raycast(sbdir * sbSpeed)) {
            sbPos = { 0,0,0 };
            sbdir = { 0,0,0 };
            TransitionFinisherState();
        }

        // ?e?????B??????SB?U???X?e?[?g??
        if (VecMath::LengthVec3(sbPos - position) <= sbSpace) {
            position = sbPos;
            sbPos = { 0,0,0 };
            sbdir = { 0,0,0 };
            TransitionFinisherState();
        }
    }
}

void Player::TransitionFinisherState() {
    // ?J??
    state = AnimeState::Finisher;
    // ???????A?j???[?V???????J
    model->AnimationStop(false);
    // ?U????????
    Vec3 front = VecMath::Normalize({ transform._31,transform._32,transform._33 });
    atkPos = { front.x,0,0 };
    atkPos = VecMath::Normalize(atkPos) * 3;
    atk = true;
}
void Player::UpdateFinisherState(float elapsedTime) {
    // ?X???[
    InputSlow(elapsedTime);
    // ????????
    if (isDead) {
        TransitionDeathState();
        return;
    }

    // ?u???[
    blurPower -= elapsedTime * blur;
    blurPower = max(blurPower, 0.0f);

    // ?A?j???[?V???????I?????????U?????????p?????d??
    bool stop = false;
    if (!model->IsPlayAnimatimon()) {
        stopTimer += elapsedTime;
        if (stopTimer >= stopTime) stop = true;
    }

    // ?A?j???[?V???????I????????????????
    if (stop) {
        // ?I?????????A?C?h????????
        TransitionIdleState();
        // ?q?b?g?X?g?b?v??????
        hitstop = false;
        // ??????????????
        weapon = true;
        //?d??????
        gravFlag = true;
        // ????????
        clock = false;
        // ?U?????u???Z?b?g
        atkPos = { 0,-100,0 };
        atk = false;
        // ???G????
        invincible = false;
        // ?J?????V?F?C?N?i???f?j??????
        CameraManager& cameraMgr = CameraManager::Instance();
        cameraMgr.SetShakeFlag(false);

        // SB???????????G???m?E
        if (sbHitEmy >= 0) {
            EnemyManager& enemyManager = EnemyManager::Instance();
            Enemy* enemy = enemyManager.GetEnemy(sbHitEmy);
            // ???????????????E??
            if(VecMath::LengthVec3(position - enemy->GetPosition()) <= 10)
                enemy->ApplyDamage(1, 0);
            sbHitEmy = -1;
        }
        // ?u???[???l
        blurPower = 0.0f;
        // ?d???p?^?C?}?[
        stopTimer = 0.0f;
    }
}

void Player::TransitionDeathState() {
    // ?J??
    state = AnimeState::Death;
    // ?X???[??
    slow = true;
    // ?d????????
    gravFlag = false;
    // ???????A?j???[?V???????????~????
    model->AnimationStop(true);
    // ??????????
    velocity = { 0,0,0 };
}
void Player::UpdateDeathState(float elapsedTime) {
    dest.destruction += elapsedTime * 10.0f;
    // ??????????????????
    if (dest.destruction >= 5.0f) {
        dest.destruction = 5.0f;
        reset = true;
        model->AnimationStop(false);
    }
}








bool Player::Raycast(Vec3 move) {

    // ??????
    float my = move.y;
    bool result = false;

    // ??????
    if (my < 0.0f) {
        // ???C???J?n???u????????????????
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // ???C???I?_???u?????????????u
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        // ???C?L???X?g???????n??????
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            if (hit.penetrate) {
                // ??????????????
                position += move;
                isGround = false;
            }
            else {
                // ?n???????n????????
                position.y = hit.position.y;
                // ???n????
                if (!isGround) OnLanding();
                isGround = true;
                result = true;
            }
        }
        else {
            // ??????????????
            position += move;
            isGround = false;
        }
    }
    // ??????
    else if (my > 0.0f) {
        // ???C???J?n???u????
        DirectX::XMFLOAT3 start = { position.x, position.y + headPos.y, position.z };
        // ???C???I?_???u?????????????u
        DirectX::XMFLOAT3 end = { position.x, position.y + headPos.y + my, position.z };
        // ???C?L???X?g???????V??????
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            if (hit.penetrate) {
                // ??????????????
                position += move;
            }
            else {
                // ?V??????????????
                position.y = hit.position.y - headPos.y;
                result = true;
            }
        }
        else {
            // ??????????????
            position += move;
        }
    }
    else {
        position += move;
    }

    // ?????????v?Z
    float velocityLengthXZ = sqrtf(move.x * move.x);
    // ????????????
    direction = VecMath::sign(velocity.x);

    if (velocityLengthXZ > 0.0f) {
        // ?????????l
        float mx = move.x;

        // ???C???J?n???u???I?_???u(??)
        DirectX::XMFLOAT3 start2 = { position.x , position.y + stepOffset + waistPos.y, position.z };
        DirectX::XMFLOAT3 end2 = { position.x + mx, position.y + stepOffset + waistPos.y, position.z };

        // ???C?L???X?g????????????
        HitResult hit;
        if (StageManager::Instance().RayCast(start2, end2, hit)) {
            // ?????????x?N?g??
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start2);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end2);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // ?????@??
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // ?????x?N?g?????@???????e                          // ?x?N?g????????????
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // ???????u???v?Z                   // v1 ?x?N?g?????? v2 ?x?N?^?[???Z   3 ???????x?N?g???????????????????? 2 ?????x?N?g?????????v?Z
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // ?????l ?x?N?g???????a??????    ?????l?@???@v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // ???????u??????????????????????????
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
    // ????????????????????
    if (!weapon) {
        SBManager& sbManager = SBManager::Instance();
        int sbCount = sbManager.GetProjectileCount();
        for (int i = 0; i < sbCount; ++i) {
            // SB????
            SB* sb = sbManager.GetProjectile(i);
            if (VecMath::LengthVec3(sb->GetMoveLen(), true) >= sbEraseLen) {
                weapon = true;
                // ??????????
                direction = VecMath::sign(sb->GetPosition().x - position.x);
                // ????????
                if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                // ?????????????????????[?v
                // ???[?v???w??
                sbdir = VecMath::Normalize(sb->GetPosition() - position);
                sbPos = sb->GetPosition();
                sbhit = true;
                sb->Destroy();
            }
        }
    }
}

// ???S???W?X?V
void Player::UpdateCenterPosition()
{
    centerPosition = position;
    centerPosition.y += height / 2.0f;
}

void Player::Launch(const Vec3& direction) {
    ID3D11Device* device = Graphics::Ins().GetDevice();
    // ????
    SBNormal* sb = new SBNormal(device, &SBManager::Instance());
    // ?????A?@?????n?_
    sb->Launch(VecMath::Normalize(direction), sbLaunchPos);
}

// ??????
void Player::FallIsDead()
{    
    // ????????????
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
        // ????????
        if (Collision::SphereVsSphere(enemy->GetCenterPosition(), swordPos + Vec3(0, -1.5f, 0), enemy->GetRadius(), atkRadius)) {
            if (enemy->GetHealth() > 0) {
                enemy->ApplyDamage(1, 0);
                // ?q?b?g?X?g?b?v
                if (!slow)hitstop = true;
                // ?J?????V?F?C?N?i???f?j
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
        // ????????
        if (Collision::SphereVsSphere(enemyBullet->GetPosition(), swordPos, enemyBullet->GetRadius(), atkRadius)) {
            if(!enemyBullet->GetReflectionFlag()) enemyBullet->SetDirection(-enemyBullet->GetDirection());
            enemyBullet->SetReflectionFlag(true);
            // ?q?b?g?X?g?b?v
            if (!slow)hitstop = true;
            SEReflect = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Reflect.wav", false);
            SEReflect.get()->Play(0.8f);
            // ?J?????V?F?C?N?i???f?j
            CameraManager& cameraMgr = CameraManager::Instance();
            if (!cameraMgr.GetShakeFlag()) {
                cameraMgr.SetShakeFlag(true);
            }

            break;
        }
    }
}
void Player::CollisionSBVsEnemies() {
    // ?G?T??
    if (!sbhit) {
        EnemyManager& enemyManager = EnemyManager::Instance();
        int enemyCount = enemyManager.GetEnemyCount();
        for (int i = 0; i < enemyCount; ++i) {
            Enemy* enemy = enemyManager.GetEnemy(i);
            // SB?T??
            SBManager& sbManager = SBManager::Instance();
            int enemyBCount = sbManager.GetProjectileCount();
            for (int j = 0; j < enemyBCount; ++j) {
                SB* sb = sbManager.GetProjectile(j);
                // ????????
                if (Collision::SphereVsSphere(enemy->GetPosition(), sb->GetPosition(), enemy->GetRadius(), atkRadius)) {
                    if (enemy->GetHealth() > 0) {
                        // ??????????
                        direction = VecMath::sign(enemy->GetPosition().x - position.x);
                        // ????????
                        if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
                        // ?G??SB?q?b?g
                        sbhit = true;
                        // ???????G????????
                        // ???@???G?????u???????E?????@?????????????????????????[?v?@???????E??
                        sbdir = VecMath::Normalize(VecMath::Subtract(enemy->GetPosition(), position));
                        // ???B?n?_
                        // ?G???A??????????????????
                        sbPos = enemy->GetPosition();
                        sbPos.x += -(VecMath::sign(enemy->GetPosition().x - position.x)) * sbSpace;
                        // ??????????
                        sb->Destroy();
                        sbHitEmy = i;
                    }
                }
            }
        }
    }
}
void Player::CollisionSBVsStage() {
    // SB?T??
    SBManager& sbManager = SBManager::Instance();
    int sbCount = sbManager.GetProjectileCount();
    for (int i = 0; i < sbCount; ++i) {
        // SB????
        SB* sb = sbManager.GetProjectile(i);
        Vec3 pos = sb->GetPosition();
        Vec3 dir = sb->GetDirection();
        float speed = sb->GetSpeed();
        HitResult hit;
        // ?X?e?[?W????????
        if (StageManager::Instance().RayCast(pos, pos + VecMath::Normalize(dir) * speed, hit)) {
            if (hit.penetrate) return;
            // ??????????
            direction = VecMath::sign(hit.position.x - position.x);
            // ????????
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // ???????G????????
            sbdir = VecMath::Normalize(VecMath::Subtract(hit.position, position));
            // ???B?n?_
            // ?G???A??????????????????
            sbPos = hit.position;
            // ?e????
            sb->Destroy();
            // stage??SB?q?b?g
            sbhit = true;
        }
        else if (DoorManager::Instance().RayCast(pos, pos + VecMath::Normalize(dir) * speed, hit)) {
            // ??????????
            direction = VecMath::sign(hit.position.x - position.x);
            // ????????
            if (direction != 0) angle.y = DirectX::XMConvertToRadians(90) * direction;
            // ???????G????????
            sbdir = VecMath::Normalize(VecMath::Subtract(hit.position, position));
            // ???B?n?_
            // ?G???A??????????????????
            sbPos = hit.position;
            // ?e????
            sb->Destroy();
            // stage??SB?q?b?g
            sbhit = true;
        }
    }
}