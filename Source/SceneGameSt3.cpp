#include "SceneGameSt3.h"
#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"
#include "EnemyShotGunner.h"
#include "EnemyMelee.h"

#include"SceneLoading.h"
#include"SceneGameSt4.h"
#include "SceneTitle.h"

#include "Menu.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain3.h"
#include "StageCollision3.h"
#include "StagePenetrate3.h"

#include "DoorManager.h"
#include "Door.h"

#include "Framework.h"

#include"EffectManager.h"
#include"Graphics/Texture.h"


// ������
void SceneGameSt3::Initialize()
{
    // ���[�h��������    
    SetLoadPercent(0.0f);
    BGM = Audio::Instance().LoadAudioSource("Data\\Audio\\BGM\\GameBGM3.wav", true);
    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();
    Graphics& gfx = Graphics::Ins();

    // �v���C���[
    player = std::make_unique<Player>(device);
    player->Init();
    player->SetSt3(true);
    player->SetPosition(Vec3(40.0f, 0.5f, 0.0f));

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �X�e�[�W

    StageManager::Create();
    StageManager::Instance().Init();

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    StageMain3* stageMain = new StageMain3(device);
    stageMain->PlayerData(player.get());
    StageManager::Instance().Register(stageMain);
    StageCollision3* stageCollision = new StageCollision3(device);
    StageManager::Instance().Register(stageCollision);
    StagePenetrate3* stagePenetrate = new StagePenetrate3(device);
    StageManager::Instance().Register(stagePenetrate);
    StageSkybox* skybox = new StageSkybox(device);
    StageManager::Instance().Register(skybox);

    // �h�A
    {
        DoorManager::Instance().Init();
        static const int DOOR_MAX = 6;
        Vec3 doorPos[DOOR_MAX] = {};
        doorPos[0] = { 18.0f,0.5f,-3.5f };
        doorPos[1] = { 18.0f,57.5f,-3.5f };
        doorPos[2] = { -185.0f,29.3f,-3.5f };
        doorPos[3] = { -250.0f,29.3f,-3.5f };
        doorPos[4] = { -220.0f,53.3f ,-3.5f };
        doorPos[5] = { -224.0f,84.7f,-3.5f };

        for (int i = 0; i < DOOR_MAX; i++)
        {
            Door* door = new Door(device);
            door->SetPos(doorPos[i]);
            door->PlayerData(player.get());
            DoorManager::Instance().Register(door);
        }
    }
    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �G�l�~�[
    {
        // �G�l�~�[���W�ݒ�
        EnemyStatusSetting();
        // �G�l�~�[������			    
        EnemyInitialize(device);
    }

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �}�E�X�J�[�\�����������ǂ���
    Input::Instance().GetMouse().SetMoveCursor(true);

    // CAMERA_SHAKE
    // TODO:02 Create a constant buffer object.
    D3D11_BUFFER_DESC buffer_desc3{};
    buffer_desc3.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
    buffer_desc3.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc3.CPUAccessFlags = 0;
    buffer_desc3.MiscFlags = 0;
    buffer_desc3.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc3, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


    cursorSprite = new Sprite(device, L"./Data/Sprites/UI/mouseTarget.png");
    Bar = new Sprite(device, L"./Data/Sprites/UI/slow.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/UI/gauge.png");
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");
    fade = new Sprite(device, L"./Data/Sprites/scene/black.png");


    Menu::Instance().Initialize();

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    Fade::Instance().Initialize();

    Goal::Instance().Init(device);
    Goal::Instance().SetGoalPos(stageMain->GetGoalPos());


    //�f�o�b�O
    //hitEffect = new Effect("Data/Effect/player_hit.efk");
    BluShader = Shaders::Ins()->GetBlurShader();
    LumiShader = Shaders::Ins()->GetLuminanceShader();

    framebuffer[0] = std::make_unique<Framebuffer>(device, gfx.GetScreenWidth(), gfx.GetScreenHeight());
    framebuffer[1] = std::make_unique<Framebuffer>(device, gfx.GetScreenWidth(), gfx.GetScreenHeight());
    framebuffer[2] = std::make_unique<Framebuffer>(device, gfx.GetScreenWidth() / 2, gfx.GetScreenHeight() / 2);
    fullScreen = std::make_unique<FullScreenQuad>(device);
    radialBlur = std::make_unique<RadialBlur>(device);
    CBBlur.initialize(device, gfx.GetDeviceContext());
    SBBlur.initialize(device, gfx.GetDeviceContext());
    LEcol.initialize(device, gfx.GetDeviceContext());

    // ���[�h�� 100%
    SetLoadPercent(10.0f);

    // �ϐ�������
    changeScene = false;
    ResetPos = { 40,0.5,0 };
    checkPoint = false;
}

// �I����
void SceneGameSt3::Finalize()
{
    // �G�l�~�[�I������	
    EnemyManager::Instance().Clear();

    // �h�A�I������
    DoorManager::Instance().Clear();

    // �X�e�[�W�I������
    StageManager::Instance().Clear();
    StageManager::Destory();

    Menu::Instance().Finalize();

    // �J�����V�F�C�N�i�ȑf�j
    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.SetShakeFlag(false);

    delete fade;
    delete enemyattack;
    delete LoadBar;
    delete Bar;
    delete cursorSprite;
    //�f�o�b�O
   // delete hitEffect;
}

// �X�V����
void SceneGameSt3::Update(float elapsedTime)
{
    BGM.get()->Play(0.6f);
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    if (Menu::Instance().GetMenuFlag() == false)
    {

        float slowElapsedTime = elapsedTime * player->GetPlaybackSpeed();
        ////TODO: �G�̐��������elapsedTime�@���������Ȃ�
        //// �q�b�g�X�g�b�v
        //slowElapsedTime = slowElapsedTime * player->GetHitStopSpeed();
        //// �X���[���[�V����
        //slowElapsedTime = slowElapsedTime * player->GetPlaybackSpeed();



        DirectX::XMFLOAT3 screenPosition;
        screenPosition.x = static_cast<float>(mouse.GetPositionX());
        screenPosition.y = static_cast<float>(mouse.GetPositionY());


        // �X�e�[�W
        StageManager::Instance().Update(slowElapsedTime);

        // �h�A
        DoorManager::Instance().Update(elapsedTime);

        // �v���C���[
        {
            player->Update(slowElapsedTime);
            // �V�t�g�u���C�N�X�V����
            SBManager::Instance().Update(slowElapsedTime);
        }

        // �J����
        {
            CameraManager& cameraMgr = CameraManager::Instance();

            cameraMgr.Update(slowElapsedTime);

            Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
            CameraManager::Instance().SetGoal(target);
        }


        // �G�l�~�[
        if (!player->GetClock())
        {
            EnemyManager::Instance().SetPlayer(player.get());
            // �\�[�g
            EnemyManager::Instance().SortLengthSq(player->GetPosition());
            // �G�l�~�[�X�V����
            EnemyManager::Instance().Update(slowElapsedTime);
            // �e�ۍX�V����
            EnemyBulletManager::Instance().Update(slowElapsedTime);

        }

        //�G�t�F�N�g�X�V����
        EffectManager::Instance().Update(slowElapsedTime);


        // �X���[���ԕ\��
        w = player->GetSlowTimer() / player->GetSlowMax();
        et = elapsedTime;
    }

    // ���Z�b�g���W�X�V
    UpdateResetPos();

    // ���Z�b�g
    if (player->GetReset()) {
        // �t�F�[�h�A�E�g
        if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);

        // �t�F�[�h�A�E�g���������
        if (Fade::Instance().GetFadeOutFinish()) {
            // ���Z�b�g
            Reset();

            // �t�F�[�h�C��
            Fade::Instance().SetFadeInFlag(true);
        }
    }
    // Menu
    Menu::Instance().Update(elapsedTime);
    // Fade
    if (Menu::Instance().GetChangeFlag())
    {
        // �t�F�[�h�A�E�g
        if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);

        // �t�F�[�h�A�E�g���������
        if (Fade::Instance().GetFadeOutFinish()) {

            // ���Z�b�g���W�����Z�b�g
            ResetPos = { 40,0.5,0 };
            checkPoint = false;

            // ���Z�b�g
            Reset();

            // �t�F�[�h�C��
            Fade::Instance().SetFadeInFlag(true);
        }
    }
    // �t�F�[�h�C���I������珉����
    if (Fade::Instance().GetFadeInFinish()) Fade::Instance().Initialize();

    // Fade
    Fade::Instance().Update(elapsedTime);
    // Goal
    Goal::Instance().SetPlayerPos(player->GetCenterPosition());
    Goal::Instance().Update(elapsedTime);

    // ���݂̃X�e�[�W�̎���ł�G�l�~�[�̐����O�̏ꍇ
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        Goal::Instance().SetCanGoal(true);
        // �S�[���Ɣ���Ƃ�         
        if (StageManager::Instance().CollisionPlayerVsNextStagePos(player->GetCenterPosition(), player->GetRadius()))
        {
            changeScene = true;
            // �t�F�[�h�A�E�g
            if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);
        }
    }
    // �t�F�[�h�A�E�g��������玟�̃V�[���ɂ�����
    if (changeScene && Fade::Instance().GetFadeOutFinish())
    {
        BGM.get()->Stop();
        // ���̃X�e�[�W�ֈڂ鏈��
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGameSt4));
    }
    
}

// �`�揈��
void SceneGameSt3::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();
    CameraManager& cameraMgr = CameraManager::Instance();

    ID3D11RenderTargetView* rtv = gfx.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = gfx.GetDepthStencilView();


    FLOAT color[] = { 0.6f,0.6f,0.6f,1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // �ʏ탌���_�����O
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //DirectX::XMFLOAT4X4 data{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    //// TODO:05 Bind the transformation matrix data to the vertex shader at register number 0.
    //dc->UpdateSubresource(constant_buffer, 0, 0, &data, 0, 0);
    //dc->VSSetConstantBuffers(3, 1, &constant_buffer);

    framebuffer[0]->clear(dc, 1.0f, 1.0f, 1.0f, 0.0f);
    framebuffer[0]->activate(dc);
    {
        // ���f���`��
        {
            // �X�e�[�W�`��
            StageManager::Instance().Render(dc, elapsedTime);
            // �X���[���o�A�G�⎩�@�ȂǏd�v�ȃI�u�W�F�N�g�ȊO���Â�����
            fade->render(dc, 0, 0, 1920, 1080, 1, 1, 1, player->GetSlowAlpha(), 0);
            // �h�A�`��
            DoorManager::Instance().Render(dc, elapsedTime);
            // �v���C���[�`��
            player->Render(dc);
            // �G�l�~�[�`��
            EnemyManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader(), player->GetSlowFlag() & !player->GetDead());
            EnemyBulletManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader(), player->GetSlowFlag() & !player->GetDead());
        }

        // �f�o�b�N
        {
            player->DrawDebugGUI();
        }

        //3D�G�t�F�N�g�`��
        {
            EffectManager::Instance().Render(cameraMgr.GetView(), cameraMgr.GetProjection());
        }
    }

    framebuffer[0]->deactivate(dc);
    CBBlur.data.BlurPower = player->GetBlurPower();
    CBBlur.data.TU = 1.0f / gfx.GetScreenWidth();
    CBBlur.data.TV = 1.0f / gfx.GetScreenHeight();
    CBBlur.applyChanges();
    dc->VSSetConstantBuffers(8, 1, CBBlur.GetAddressOf());
    dc->PSSetConstantBuffers(8, 1, CBBlur.GetAddressOf());
    dc->GSSetConstantBuffers(8, 1, CBBlur.GetAddressOf());

    framebuffer[1]->clear(dc);
    framebuffer[1]->activate(dc);
    radialBlur->blit(dc, framebuffer[0]->shaderResourceViews[0].GetAddressOf(), 0, 1);
    framebuffer[1]->deactivate(dc);

    LEcol.data.rgb = LErgb;
    LEcol.applyChanges();
    dc->VSSetConstantBuffers(5, 1, LEcol.GetAddressOf());
    dc->PSSetConstantBuffers(5, 1, LEcol.GetAddressOf());
    dc->GSSetConstantBuffers(5, 1, LEcol.GetAddressOf());

    SBBlur.data.sigma = sigma;
    SBBlur.data.intensity = intensity;
    SBBlur.data.dummy0 = exp;
    SBBlur.applyChanges();
    dc->VSSetConstantBuffers(4, 1, SBBlur.GetAddressOf());
    dc->PSSetConstantBuffers(4, 1, SBBlur.GetAddressOf());
    dc->GSSetConstantBuffers(4, 1, SBBlur.GetAddressOf());

    framebuffer[2]->clear(dc);
    framebuffer[2]->activate(dc);
    fullScreen->blit(dc, framebuffer[1]->shaderResourceViews[0].GetAddressOf(), 0, 1, LumiShader.GetPixelShader().Get());
    framebuffer[2]->deactivate(dc);

    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> shader_resource_views[2] =
    { framebuffer[1]->shaderResourceViews[0].Get(), framebuffer[2]->shaderResourceViews[0].Get() };
    radialBlur->blit(dc, shader_resource_views->GetAddressOf(), 0, 2, BluShader.GetPixelShader().Get());


    // 2D�`��
    {
        // �U���\���`��
        RenderEnemyAttack();

        Mouse& mouse = Input::Instance().GetMouse();
        GamePad& pad = Input::Instance().GetGamePad();
        if (Input::Instance().GetGamePad().GetUseKeybord())
        {
            // �}�E�X�ʒu
            Vec2 screenPosition = { static_cast<float>(mouse.GetPositionX()) ,static_cast<float>(mouse.GetPositionY()) };
            cursorSprite->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64);
        }
        else
        {
            // ���[�v�X�g���C�N�����������ݒ�
            Vec3 WSdirection = { -pad.GetAxisLX(), pad.GetAxisLY(), 0 };

            // �r���[�|�[�g
            D3D11_VIEWPORT viewport;
            UINT numViewports = 1;
            dc->RSGetViewports(&numViewports, &viewport);
            // �ϊ��s��
            DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
            DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
            DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
            DirectX::XMFLOAT3 worldPosition = player->GetCenterPosition() + (WSdirection * 20.0f);
            DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
            // ���[���h���W����X�N���[�����W�֕ϊ�
            DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(WorldPosition,
                viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth, Projection, View, World);
            // �X�N���[�����W
            DirectX::XMFLOAT2 screenPosition;
            DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

            if (WSdirection.x != 0 && WSdirection.y != 0)cursorSprite->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64);
        }
        // UI
        Bar->render(dc, 400, 500, 500, 225, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 570, 578.75f, 290 * w, 58.5f, 1.0f, 1.0f, 1.0f, 0.8f, 0);

        // Goal
        Goal::Instance().Render(dc);

        // ���j���[
        Menu::Instance().Render(elapsedTime);

        // �t�F�[�h�p
        Fade::Instance().Render(elapsedTime);

    }


#ifdef USE_IMGUI   


    ImGui::Begin("ImGUI");

    // CAMERA_SHAKE
    // TODO:07 Adjust the maximum amount of rotation(max_skew) and movement(max_sway) of the camera.
    ImGui::SliderFloat("max_sway [pixel]", &max_sway, 0.0f, 64.0f);
    ImGui::SliderFloat("max_skew [degree]", &max_skew, 0.0f, 10.0f);
    // TODO:12 Defines the amount of seed shifting factor for perlin noise.
    ImGui::SliderFloat("seed_shifting_factor", &seed_shifting_factor, 0.0f, 10.0f);

    ImGui::SliderFloat("elapsedTime", &et, 0.0f, 1.0f);

    bool sh = cameraMgr.GetShakeFlag();
    ImGui::Checkbox("shakeFlag", &sh);

    ImGui::SliderFloat("gaussian_sigma", &sigma, 0, 2);
    ImGui::SliderFloat("bloom_intensity", &intensity, 0, 0.5f);
    ImGui::SliderFloat("expo", &exp, 0, 10);
    ImGui::SliderFloat("LEx", &LErgb.x, 0, 1);
    ImGui::SliderFloat("LEy", &LErgb.y, 0, 1);
    ImGui::SliderFloat("LEz", &LErgb.z, 0, 1);

    ImGui::End();

    /* ImGui::SetNextWindowPos(ImVec2(0, 10), ImGuiCond_FirstUseEver);
     ImGui::SetNextWindowSize(ImVec2(30, 30), ImGuiCond_FirstUseEver);

     if (ImGui::Begin("SRV", nullptr, ImGuiWindowFlags_None))
     {
         ImGui::Image(shaderResourceViews[0], { 320, 180 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
         ImGui::Image(shaderResourceViews[1], { 320, 180 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
         ImGui::Image(shaderResourceViews[2], { 320, 180 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
     }
     ImGui::End();*/
#endif

}

// player�����񂾂Ƃ� ���̃��Z�b�g�p
void SceneGameSt3::Reset()
{
    // �S�[���s��
    Goal::Instance().SetCanGoal(false);

    // ���܂Ȃ�
    EnemyBulletManager::Instance().Clear();
    SBManager::Instance().Clear();
    // �G�h�� �|�W�V�������Z�b�g
    EnemyManager::Instance().Init();
    EnemyManager::Instance().EnemyReset();
    // �h�A���Z�b�g
    DoorManager::Instance().Init();

    // �v���C���[�h�� �|�W�V�������Z�b�g
    player->Init();
    player->SetPosition(ResetPos);
    Menu::Instance().Initialize();

    if (checkPoint)
    {
        for (int i = 0; i < EnemyManager::Instance().GetEnemyCount(); i++)
        {
            Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
            if (enemy->GetPosition().x >= -125)
            {
                EnemyManager::Instance().EnemyKill(i);                
                //EnemyManager::Instance().Remove(enemy);
            }
        }      
    }


}

// �G�̏�����
void SceneGameSt3::EnemyInitialize(ID3D11Device* device)
{
    bool melee, gunner, shotgunner;

    for (int i = 0; i < ENEMY_MAX; i++)
    {
        if (ENEMY_MAX == i)
        {
            // ���[�h���X�V
            AddLoadPercent(1.0f);
        }
                
        // �ߐ� 01347
        if (i == 0 || i == 1 || i == 3 || i == 5 || i == 7 || i == 10)
        {
            melee = true;
            gunner = shotgunner = false;
        }
        // ���u 289
        else if (i == 2 || i == 8 || i == 9)
        {
            gunner = true;
            melee = shotgunner = false;
        }
        // ���u(�U) 56
        else if (i == 4 || i == 6 )
        {
            shotgunner = true;
            gunner = melee = false;
        }        

        // �ߐ�
        if (melee)
        {
            EnemyMelee* melee = new EnemyMelee(device);

            // ���W�Z�b�g
            melee->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            melee->PositionInitialize();
            //������邩�ǂ���
            melee->SetInitialWalk(enemyWalk[i]);
            melee->WalkFlagInitialize();
            // �O���[�v�ԍ��Z�b�g
            melee->SetInitialGroupNum(enemyGroup[i]);
            melee->GroupNumInitialize();
            // �����Z�b�g
            melee->SetInitialDirection(enemyDirection[i]);
            melee->DirectionInitialize();

            EnemyManager::Instance().Register(melee);
        }
        else if (shotgunner)
        {
            EnemyShotGunner* gunner = new EnemyShotGunner(device);

            // ���W�Z�b�g
            gunner->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            gunner->PositionInitialize();
            //������邩�ǂ���
            gunner->SetInitialWalk(enemyWalk[i]);
            gunner->WalkFlagInitialize();
            // �O���[�v�ԍ��Z�b�g
            gunner->SetInitialGroupNum(enemyGroup[i]);
            gunner->GroupNumInitialize();
            // �����Z�b�g
            gunner->SetInitialDirection(enemyDirection[i]);
            gunner->DirectionInitialize();

            EnemyManager::Instance().Register(gunner);
        }
        else if (gunner)
        {
            EnemyGunner* gunner = new EnemyGunner(device);

            // ���W�Z�b�g
            gunner->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            gunner->PositionInitialize();
            //������邩�ǂ���
            gunner->SetInitialWalk(enemyWalk[i]);
            gunner->WalkFlagInitialize();
            // �O���[�v�ԍ��Z�b�g
            gunner->SetInitialGroupNum(enemyGroup[i]);
            gunner->GroupNumInitialize();
            // �����Z�b�g
            gunner->SetInitialDirection(enemyDirection[i]);
            gunner->DirectionInitialize();

            EnemyManager::Instance().Register(gunner);
        }

    }

    EnemyManager::Instance().Init();
}

// �G�l�~�[���W�ݒ�
void SceneGameSt3::EnemyStatusSetting()
{
    enemyPos[0] = { -53.0f,0.6f };
    enemyPos[1] = { -197.0f,29.5f };
    enemyPos[2] = { -242.0,29.0f };
    enemyPos[3] = { -285.0f,29.0f };
    enemyPos[4] = { -305.0f,54.0f };
    enemyPos[5] = { -350.0f, 54.0f };
    enemyPos[6] = { -270.0f,54.0f };
    enemyPos[7] = { -255.0f,54.0f };
    enemyPos[8] = { -187.0f,54.0f };
    enemyPos[9] = { -150.0f,54.0f };
    enemyPos[10] = { 11.0f,0.5f };

    enemyGroup[0] = 0;
    enemyGroup[1] = 1;
    enemyGroup[2] = 1;
    enemyGroup[3] = 2;
    enemyGroup[4] = 3;
    enemyGroup[5] = 3;
    enemyGroup[6] = 4;
    enemyGroup[7] = 4;
    enemyGroup[8] = 5;
    enemyGroup[9] = 5;
    enemyGroup[10] = 6;

    enemyWalk[0] = false;
    enemyWalk[1] = true;
    enemyWalk[2] = false;
    enemyWalk[3] = false;
    enemyWalk[4] = false;
    enemyWalk[5] = false;
    enemyWalk[6] = false;
    enemyWalk[7] = false;
    enemyWalk[8] = false;
    enemyWalk[9] = false;
    enemyWalk[10] = false;

    enemyDirection[0] = false;
    enemyDirection[1] = false;
    enemyDirection[2] = true;
    enemyDirection[3] = true;
    enemyDirection[4] = true;
    enemyDirection[5] = false;
    enemyDirection[6] = true;
    enemyDirection[7] = true;
    enemyDirection[8] = false;
    enemyDirection[9] = false;
    enemyDirection[10] = false;

}


// �G�l�~�[�U���\���`��
void SceneGameSt3::RenderEnemyAttack()
{

    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();

    // �r���[�|�[�g
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);
    // �ϊ��s��
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
    // �S�Ă̓G�̓����HP�Q�[�W��\��
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        DirectX::XMFLOAT3 worldPosition = enemy->GetCenterPosition();
        worldPosition.y += enemy->GetHeight();
        DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
        // ���[���h���W����X�N���[�����W�֕ϊ�
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
        // �X�N���[����3
        DirectX::XMFLOAT2 screenPosition;
        DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

        if (enemy->GetIsSearch() && !enemy->GetIsAttack() && enemy->GetHealth() > 0)
        {
            // �U���\���`��
            enemyattack->render(dc, screenPosition.x - 32, screenPosition.y - 64, 64, 64, 1, 1, 1, 1, 0);
        }
    }
}

// ���Z�b�g���W�X�V
void SceneGameSt3::UpdateResetPos()
{
    // �v���C���[�̍��W�ɂ���ă��Z�b�g�����Ƃ��̍��W�ς���  
    Vec3 collision = { -125.0f,30.0f,0.0f };
    // �X�t�B�A�ŃR���W�����Ƃ�
    if (Collision::SphereVsSphere(collision, player->GetCenterPosition(), 6.0f, player->GetRadius()))
    {
        // ��ꂽ�ǂ̂Ƃ���������X�V����
        ResetPos = { -125.0f,37.0f,0 };
        checkPoint = true;
    }

    // ���̎��_�Ŏ���ł�G�l�~�[�͎��񂾂܂܂ɂ��� 
}
