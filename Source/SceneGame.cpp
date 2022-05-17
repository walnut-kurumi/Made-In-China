#include "SceneGame.h"

#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"
#include "EnemyMelee.h"

#include"SceneLoading.h"
#include"SceneGameSt2.h"

#include "Menu.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain1.h"
#include "StageCollision1.h"
#include "StagePenetrate1.h"

#include "DoorManager.h"
#include "Door.h"

#include "Framework.h"

#include"EffectManager.h"
#include"Graphics/Texture.h"


// ������
void SceneGame::Initialize()
{
    // ���[�h��������    
    SetLoadPercent(0.0f);

    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();
    Graphics& gfx = Graphics::Ins();

    // �v���C���[
    player = std::make_unique<Player>(device);
    player->Init(); 

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �X�e�[�W
    {
        StageManager::Create();
        StageManager::Instance().Init();

        // ���[�h���X�V
        AddLoadPercent(1.0f);

        StageMain1* stageMain = new StageMain1(device);
        stageMain->PlayerData(player.get());
        StageManager::Instance().Register(stageMain);
        StageCollision1* stageCollision = new StageCollision1(device);
        StageManager::Instance().Register(stageCollision);
        StagePenetrate1* stagePenetrate = new StagePenetrate1(device);
        StageManager::Instance().Register(stagePenetrate);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    }
    // �h�A
    {        
        DoorManager::Instance().Init();

        Door* door = new Door(device);
        door->SetPos(Vec3(-54.0f, 29.6f , -3.5f));
        door->PlayerData(player.get());        
        DoorManager::Instance().Register(door);            
    }
    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �G�l�~�[
    {
        // �G�l�~�[���W�ݒ�
        EnemyPositionSetting();
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


    Bar = new Sprite(device, L"./Data/Sprites/UI/slow.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/UI/gauge.png");   
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");
    fade = new Sprite(device, L"./Data/Sprites/scene/black.png");

    Menu::Instance().Initialize();

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    Fade::Instance().Initialize();

    //�f�o�b�O
    //hitEffect = new Effect("Data/Effect/player_hit.efk");
    BluShader = Shaders::Ins()->GetBlurShader();

    framebuffer[0] = std::make_unique<Framebuffer>(device, gfx.GetScreenWidth(), gfx.GetScreenHeight());
    framebuffer[1] = std::make_unique<Framebuffer>(device, gfx.GetScreenWidth()/2, gfx.GetScreenHeight()/2);
    radialBlur = std::make_unique<RadialBlur>(device);
    CBBlur.initialize(device, gfx.GetDeviceContext());
    SBBlur.initialize(device, gfx.GetDeviceContext());
    // ���[�h�� 100%
    SetLoadPercent(10.0f);

    // �ϐ�������
    changeScene = false;
}

// �I����
void SceneGame::Finalize()
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

    delete enemyattack;
    delete LoadBar;
    delete Bar;
    delete fade;
    //�f�o�b�O
   // delete hitEffect;
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    if (Menu::Instance().GetMenuFlag() == false)
    {

        float slowElapsedTime = elapsedTime * player->GetPlaybackSpeed();
        // �q�b�g�X�g�b�v
        slowElapsedTime = slowElapsedTime * player->GetHitStopSpeed();
        // �X���[���[�V����
        slowElapsedTime = slowElapsedTime * player->GetPlaybackSpeed();



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
        if(!player->GetClock())
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
    // �t�F�[�h�C���I������珉����
    if (Fade::Instance().GetFadeInFinish()) Fade::Instance().Initialize();

    // Menu
    Menu::Instance().Update(elapsedTime);
    // Fade
    Fade::Instance().Update(elapsedTime);
    

    // ���݂̃X�e�[�W�̎���ł�G�l�~�[�̐����O�̏ꍇ
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
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
        // ���̃X�e�[�W�ֈڂ鏈��
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGameSt2));
    }

}

// �`�揈��
void SceneGame::Render(float elapsedTime)
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
 
    framebuffer[0]->clear(dc,1.0f,1.0f,1.0f,0.0f);
    framebuffer[0]->activate(dc);
    {
        // ���f���`��
        {
            // �X�e�[�W�`��
            StageManager::Instance().Render(dc, elapsedTime);
            // �h�A�`��
            DoorManager::Instance().Render(dc, elapsedTime);

            // �X���[���o�A�G�⎩�@�ȂǏd�v�ȃI�u�W�F�N�g�ȊO���Â�����
            fade->render(dc, 0, 0, 1920, 1080, 1, 1, 1, player->GetSlowAlpha(), 0);

            // �v���C���[�`��
            player->Render(dc);
            // �G�l�~�[�`��
            EnemyManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());
            EnemyBulletManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());
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

    SBBlur.data.sigma = sigma;
    SBBlur.data.intensity = intensity;
    SBBlur.data.dummy0 = exp;
    SBBlur.applyChanges();
    dc->VSSetConstantBuffers(4, 1, SBBlur.GetAddressOf());
    dc->PSSetConstantBuffers(4, 1, SBBlur.GetAddressOf());
    dc->GSSetConstantBuffers(4, 1, SBBlur.GetAddressOf());

    framebuffer[1]->clear(dc);
    framebuffer[1]->activate(dc);
    radialBlur->blit(dc, framebuffer[0]->shaderResourceViews[0].GetAddressOf(), 0, 1);
    framebuffer[1]->deactivate(dc);
    
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> shader_resource_views[2] = 
        { framebuffer[0]->shaderResourceViews[0].Get(), framebuffer[1]->shaderResourceViews[0].Get() };
    radialBlur->blit(dc, shader_resource_views->GetAddressOf(), 0, 2, BluShader.GetPixelShader().Get());

    // 2D�`��
    {
        // �U���\���`��
        RenderEnemyAttack();

        // UI
        Bar->render(dc, 0, 0, 600, 300, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 208, 105, 344 * w, 78, 1.0f, 1.0f, 1.0f, 1.0f, 0);

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
void SceneGame::Reset()
{
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
    

}

// �G�̏�����
void SceneGame::EnemyInitialize(ID3D11Device* device)
{
    for (int i = 0; i < ENEMY_MAX; i++)
    {        
        if (ENEMY_MAX == i)
        {
            // ���[�h���X�V
            AddLoadPercent(1.0f);
        }

        // �ߐ�
        if (i == 0)
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
        else
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
void SceneGame::EnemyPositionSetting()
{

    enemyPos[0] = { -6.5f,29.5f };
    enemyPos[1] = { -48.0f,29.5f };
    enemyPos[2] = { -140.0f,29.5f };

    enemyGroup[0] =0;
    enemyGroup[1] =0;
    enemyGroup[2] =1;
    
    enemyWalk[0] = true;
    enemyWalk[1] = false;
    enemyWalk[2] = true;
   
    enemyDirection[0] = false;
    enemyDirection[1] = true;
    enemyDirection[2] = true;
}


// �G�l�~�[�U���\���`��
void SceneGame::RenderEnemyAttack()
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
        // �X�N���[�����W
        DirectX::XMFLOAT2 screenPosition;
        DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);                

        if (enemy->GetIsAttack() && enemy->GetHealth() > 0)
        {
            // �U���\���`��
            enemyattack->render(dc, screenPosition.x - 32, screenPosition.y - 64, 64, 64, 1, 1, 1, 1, 0);
        }
    }   
}