#include "SceneGame.h"
#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"
#include "EnemyMelee.h"

#include"SceneLoading.h"
#include"SceneClear.h"
#include"SceneOver.h"

#include "Menu.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain.h"
#include "StageCollision.h"

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

        StageMain* stageMain = new StageMain(device);
        stageMain->PlayerData(player.get());
        StageManager::Instance().Register(stageMain);
        StageCollision* stageCollision = new StageCollision(device);
        StageManager::Instance().Register(stageCollision);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    }
    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �G�l�~�[���W�ݒ�
    EnemyPositionSetting();

    // �G�l�~�[������			    
    EnemyInitialize(device);

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


    Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");   
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");

    Menu::Instance().Initialize();

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    Fade::Instance().Initialize();

    //�f�o�b�O
    //hitEffect = new Effect("Data/Effect/player_hit.efk");

    // ���[�h�� 100%
    SetLoadPercent(10.0f);

    framebuffer = std::make_unique<Framebuffer>(device, gfx.GetScreenWidth(), gfx.GetScreenHeight());
    radialBlur = std::make_unique<RadialBlur>(device);
    CBBlur.initialize(device, gfx.GetDeviceContext());
}

// �I����
void SceneGame::Finalize()
{
    // �G�l�~�[�I������	
    EnemyManager::Instance().Clear();
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
        //TODO: �G�̐��������elapsedTime�@���������Ȃ�
        // �q�b�g�X�g�b�v
        slowElapsedTime = slowElapsedTime * player->GetHitStopSpeed();
        // �X���[���[�V����
        slowElapsedTime = slowElapsedTime * player->GetPlaybackSpeed();



        DirectX::XMFLOAT3 screenPosition;
        screenPosition.x = static_cast<float>(mouse.GetPositionX());
        screenPosition.y = static_cast<float>(mouse.GetPositionY());


        // �X�e�[�W
        StageManager::Instance().Update(slowElapsedTime);


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
            CameraManager::Instance().SetTarget(target);
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
    //if (player->GetHealth() <= 0)// ||  gamePad.GetButtonDown() & GamePad::BTN_Y)
    //{
    //    // �t�F�[�h�A�E�g
    //    if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);

    //    // �t�F�[�h�A�E�g���������
    //    if (Fade::Instance().GetFadeOutFinish())
    //    {
    //        // ���Z�b�g
    //        // �f�o�b�O�p�ŏ����Ă�
    //        Reset();

    //        // �t�F�[�h�C��
    //        Fade::Instance().SetFadeInFlag(true);
    //    }
    //}        
    // �t�F�[�h�C���I������珉����
    if (Fade::Instance().GetFadeInFinish())Fade::Instance().Initialize();

    // Menu
    Menu::Instance().Update(elapsedTime);
    // Fade
    Fade::Instance().Update(elapsedTime);

    //�G�t�F�N�g�f�o�b�O
   /* const mouseButton mouseClick =
        Mouse::BTN_LEFT;
    if(mouse.GetButtonDown() & mouseClick && a == false)
        handle = hitEffect->Play(player->GetPosition()); a = true; ti = 0;

    if (ti %= 300)
        hitEffect->Stop(handle); a = false;

    ti++;*/

    // TODO ���݂̃X�e�[�W�̎���ł�G�l�~�[�̐����O�̏ꍇ ���̃X�e�[�W��
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        // ���̃X�e�[�W�ֈڂ鏈��
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneClear));
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
 
    framebuffer->clear(dc);
    framebuffer->activate(dc);
    {
        // ���f���`��
        {
            // �X�e�[�W�`��
            StageManager::Instance().Render(dc, elapsedTime);
            // �v���C���[�`��
            player->Render(dc);
            // �G�l�~�[�`��
            EnemyManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());
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
    framebuffer->deactivate(dc);
    CBBlur.data.BlurPower = player->GetBlurPower();
    CBBlur.data.TU = 1.0f / gfx.GetScreenWidth();
    CBBlur.data.TV = 1.0f / gfx.GetScreenHeight();
    CBBlur.applyChanges();
    dc->VSSetConstantBuffers(8, 1, CBBlur.GetAddressOf());
    dc->PSSetConstantBuffers(8, 1, CBBlur.GetAddressOf());
    dc->GSSetConstantBuffers(8, 1, CBBlur.GetAddressOf());
    radialBlur->blit(dc, framebuffer->shader_resource_views[0].GetAddressOf(), 0, 1);

    // 2D�`��
    {
        // �U���\���`��
        RenderEnemyAttack();

        // UI
        Bar->render(dc, 600, 650, 620, 25, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 605, 652, 605 * w, 21, 1.0f, 1.0f, 1.0f, 1.0f, 0);

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

   /* ImGui::SliderFloat("gaussian_sigma", &sigma, -10, 1);
    ImGui::SliderFloat("bloom_intensity", &intensity, -10, 1);
    ImGui::SliderFloat("expo", &expo, 0, 10);
    if (ImGui::TreeNode("smoothstep"))
    {
        ImGui::SliderFloat("x", &rgb.x, 0, 1);
        ImGui::SliderFloat("y", &rgb.y, 0, 1);
        ImGui::SliderFloat("z", &rgb.z, 0, 1);

        ImGui::TreePop();
    }*/

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

    // �v���C���[�h�� �|�W�V�������Z�b�g
    player->Init();
    

}

// �G�̏�����
void SceneGame::EnemyInitialize(ID3D11Device* device)
{
    for (int i = 0; i < ENEMY_MAX; i++)
    {
        if (ENEMY_MAX / 2 == i)
        {
            // ���[�h���X�V
            AddLoadPercent(1.0f);
        }
        if (ENEMY_MAX == i)
        {
            // ���[�h���X�V
            AddLoadPercent(1.0f);
        }

        // �ߐڂƉ��u�����݂�
        if (i % 2 == 0)
        {
            EnemyGunner* gunner = new EnemyGunner(device);
            gunner->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            gunner->PositionInitialize();

            //������邩�ǂ���
            gunner->SetInitialWalk(enemyWalk[i]);
            gunner->WalkFlagInitialize();

            // �O���[�v�ԍ��Z�b�g
            gunner->SetInitialGroupNum(enemyGroup[i]);
            gunner->GroupNumInitialize();

            EnemyManager::Instance().Register(gunner);
        }
        else
        {
            EnemyMelee* melee = new EnemyMelee(device);
            melee->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            melee->PositionInitialize();

            //������邩�ǂ���
            melee->SetInitialWalk(enemyWalk[i]);
            melee->WalkFlagInitialize();

            // �O���[�v�ԍ��Z�b�g
            melee->SetInitialGroupNum(enemyGroup[i]);
            melee->GroupNumInitialize();

            EnemyManager::Instance().Register(melee);
        }
    }

    EnemyManager::Instance().Init();
}

// �G�l�~�[���W�ݒ�
void SceneGame::EnemyPositionSetting()
{

    enemyPos[0] = {-75,0.5f};
    enemyPos[1] = {-85,0.5f};
    enemyPos[2] = {-105,0.5f};
    enemyPos[3] = {-120,0.5f};
    enemyPos[4] = {-40,20.5f};
    enemyPos[5] = {4,42.5f};
    enemyPos[6] = {-65,55.5f};
    enemyPos[7] = {-130,85.5f};
    enemyPos[8] = {20,70.5f};    


    enemyGroup[0] =0;
    enemyGroup[1] =0;
    enemyGroup[2] =0;
    enemyGroup[3] =0;  
    enemyGroup[4] =1;
    enemyGroup[5] =1;
    enemyGroup[6] =2;
    enemyGroup[7] =2;
    enemyGroup[8] =2;

    enemyWalk[0] = true;
    enemyWalk[1] = true;
    enemyWalk[2] = true;
    enemyWalk[3] = true;
    enemyWalk[4] = false;
    enemyWalk[5] = false;
    enemyWalk[6] = false;
    enemyWalk[7] = false;
    enemyWalk[8] = false;
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