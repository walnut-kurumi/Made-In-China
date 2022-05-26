#include "SceneTutorial.h"
#include "Camera/CameraManager.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"
#include "EnemyShotGunner.h"
#include "EnemyMelee.h"

#include "SceneManager.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneClear.h"
#include "SceneTitle.h"

#include "Menu.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain0.h"
#include "StageCollision0.h"


#include "Framework.h"

#include "EffectManager.h"
#include "Graphics/Texture.h"




// ������
void SceneTutorial::Initialize()
{
    // ���[�h��������    
    SetLoadPercent(0.0f);
    BGM = Audio::Instance().LoadAudioSource("Data\\Audio\\BGM\\GameBGM1.wav", true);
    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();
    Graphics& gfx = Graphics::Ins();

    // �v���C���[
    player = std::make_unique<Player>(device);
    // �`���[�g���A������
    player->SetIsTutorial(true);
    player->Init();
    player->SetPosition(Vec3(-19, 35, 0));

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �X�e�[�W
    
        StageManager::Create();
        StageManager::Instance().Init();

        // ���[�h���X�V
        AddLoadPercent(1.0f);

        StageMain0* stageMain = new StageMain0(device);
        stageMain->PlayerData(player.get());
        StageManager::Instance().Register(stageMain);
        StageCollision0* stageCollision = new StageCollision0(device);
        StageManager::Instance().Register(stageCollision);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    
    
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


    cursorSprite= new Sprite(device, L"./Data/Sprites/UI/mouseTarget.png");
    Bar         = new Sprite(device, L"./Data/Sprites/UI/slow.png");
    LoadBar     = new Sprite(device, L"./Data/Sprites/UI/gauge.png");
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");
    fade        = new Sprite(device, L"./Data/Sprites/scene/black.png");

    KeyA        = new Sprite(device, L"./Data/Sprites/UI/Keybord/KeyA.png");
    KeyD        = new Sprite(device, L"./Data/Sprites/UI/Keybord/KeyD.png");
    KeySPACE    = new Sprite(device, L"./Data/Sprites/UI/Keybord/KeySPACE.png");
    KeySHIFT    = new Sprite(device, L"./Data/Sprites/UI/Keybord/KeySHIFT.png");
    LeftClick   = new Sprite(device, L"./Data/Sprites/UI/Keybord/MouseLClick.png");
    RightClick  = new Sprite(device, L"./Data/Sprites/UI/Keybord/MouseRClick.png");

    ButtonA     = new Sprite(device, L"./Data/Sprites/UI/Controller/ButtonA.png");
    ButtonX     = new Sprite(device, L"./Data/Sprites/UI/Controller/ButtonX.png");
    Stick       = new Sprite(device, L"./Data/Sprites/UI/Controller/Stick.png");
    ButtonLT    = new Sprite(device, L"./Data/Sprites/UI/Controller/ButtonLT.png");
    ButtonRT    = new Sprite(device, L"./Data/Sprites/UI/Controller/ButtonRT.png");


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
    isTutorial = true;
    isPause = false;
    camTargetPos = { -19,5,0 };
    cameraTargetChange = false;    
    isSlow = false;    
    changeScene = false;

    radian = 0.0f;
    tutorialTick = 0;	// �A�j���[�V�����p�`�b�N	
    stickAnim = 0;	// �A�j���[�V����
    isKeybord = false;

    renderSlow = false;
    renderAttack = false;
    renderMove = false;
    renderJump = false;
    renderSB = false;

    // �ŏ��̓v���C���[����s�� �X���[���͂��Ēe���Ԃ��Ă��瓮����   
    player->SetIsControl(false);
    player->SetCanSlow(false);
    player->SetCanAttack(false);
}

// �I����
void SceneTutorial::Finalize()
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

    delete KeyA;
    delete KeyD;
    delete KeySPACE;
    delete KeySHIFT;
    delete LeftClick;
    delete RightClick;
    delete ButtonA;
    delete ButtonX;
    delete Stick;
    delete ButtonLT;
    delete ButtonRT;

    delete enemyattack;
    delete LoadBar;
    delete Bar;
    delete fade;
    delete cursorSprite;
}

// �X�V����
void SceneTutorial::Update(float elapsedTime)
{
    BGM.get()->Play(0.5f);
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    // �L�[�{�[�h�g�p��Ԃ̎擾
    if (mouse.GetButtonDown() & Mouse::BTN_LEFT || mouse.GetButtonDown() & Mouse::BTN_RIGHT)gamePad.SetUseKeybord(true);
    isKeybord = gamePad.GetUseKeybord();

    if (Menu::Instance().GetMenuFlag() == false)
    {
        float slowElapsedTime = 0;
        if (!isPause) {
            slowElapsedTime = elapsedTime * player->GetPlaybackSpeed();            
        }
        DirectX::XMFLOAT3 screenPosition;
        screenPosition.x = static_cast<float>(mouse.GetPositionX());
        screenPosition.y = static_cast<float>(mouse.GetPositionY());

        // �`���[�g���A��
        {
            // ���n������X���[�ł���
            if (player->Ground() && isTutorial)
            {
                isPause = true;                
                player->SetCanSlow(true);

                // ��������F�X���[
                renderSlow = true;
            }            
            // �X���[���͂����炻�̂܂܃X���[
            if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER && isTutorial && player->GetCanSlow())
            {
                isSlow = true;
                isPause = false;                
            }
            if (isSlow && isTutorial)slowElapsedTime = elapsedTime * 0.25f;

            // �e�~�߂�
            for (int i = 0; i < EnemyBulletManager::Instance().GetProjectileCount(); i++)
            {
                float posX = EnemyBulletManager::Instance().GetProjectile(i)->GetPosition().x;
                if (posX <= -15.0f && isTutorial)
                {
                    EnemyBulletManager::Instance().GetProjectile(i)->SetIsMove(false);
                    isPause = true;
                    player->SetCanAttack(true);
                    
                    renderSlow = false;
                    // ��������F�U��
                    renderAttack = true;                   
                }
            }

            // ���ɗ�������
            if (player->GetPosition().x < -30.0f && renderMove)
            {
                renderMove = false;
                // ��������F�W�����v
                renderJump = true;
            }
            else if (player->GetPosition().x < -60.0f && renderJump)
            {
                renderJump = false;
            }
            // SB�ʒu�ɂ�����
            if (player->GetPosition().x < -85.0f && player->GetPosition().x > -140.0f && !renderSB)
            {                
                // ��������FSB
                renderSB = true;
            }
            else if((player->GetPosition().x > -85.0f || player->GetPosition().x < -140.0f ))
            {                                
                renderSB = false;
            }            
        }

        // �X�e�[�W
        StageManager::Instance().Update(slowElapsedTime);

        // �v���C���[
        {            
            player->Update(slowElapsedTime);
            player->SetSlowFixation(isSlow);
            player->SetIsTutorial(isTutorial);
            // �V�t�g�u���C�N�X�V����
            SBManager::Instance().Update(slowElapsedTime);

            
            // �`���[�g���A���I���
            if (player->GetIsAtk() && isTutorial) 
            {                
                isTutorial = false;
                isPause = false;
                isSlow = false;

                // �J�����̃^�[�Q�b�g�ς���
                cameraTargetChange = true;

                // ����\
                player->SetIsControl(true);
                player->SetSlowFixation(isSlow);

                // �e������
                for (int i = 0; i < EnemyBulletManager::Instance().GetProjectileCount(); i++)
                {
                    EnemyBulletManager::Instance().GetProjectile(i)->SetIsMove(true);
                }

                renderAttack = false;
                // ��������F�ړ�
                renderMove = true;
            }
        }

        // �J����
        {
            CameraManager& cameraMgr = CameraManager::Instance();

            cameraMgr.Update(slowElapsedTime);

            Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
            if(cameraTargetChange) CameraManager::Instance().SetGoal(target);
            else CameraManager::Instance().SetGoal(camTargetPos);
        }


        // �G�l�~�[
        if (!player->GetClock())
        {
            EnemyManager::Instance().SetPlayer(player.get());           
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
    // Goal
    Goal::Instance().SetPlayerPos(player->GetCenterPosition());
    Goal::Instance().Update(elapsedTime);


    // ���݂̃X�e�[�W�̎���ł�G�l�~�[�̐����O�̏ꍇ
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        // �S�ł����Ă�ꍇ������������Ȃ�
        renderSB = false;
        Goal::Instance().SetCanGoal(true);

        // �S�[���Ɣ���Ƃ�         
        if (StageManager::Instance().CollisionPlayerVsNextStagePos(player->GetCenterPosition(), player->GetRadius()))
        {
            changeScene = true;
            // �t�F�[�h�A�E�g
            if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);            
        }
    }
    if(Menu::Instance().GetChangeFlag())
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
    }
    // �t�F�[�h�A�E�g��������玟�̃V�[���ɂ�����
    if (changeScene && Fade::Instance().GetFadeOutFinish())
    {
        BGM.get()->Stop();
        // ���̃X�e�[�W�ֈڂ鏈��
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
    }
}

// �`�揈��
void SceneTutorial::Render(float elapsedTime)
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

    framebuffer[0]->clear(dc, 1.0f, 1.0f, 1.0f, 0.0f);
    framebuffer[0]->activate(dc);
    {
        // ���f���`��
        {
            // �X�e�[�W�`��
            StageManager::Instance().Render(dc, elapsedTime);           

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

        // ��������p
        RenderTutorial();

        // �}�E�X�ʒu
        Mouse& mouse = Input::Instance().GetMouse();
        Vec2 screenPosition = { static_cast<float>(mouse.GetPositionX()) ,static_cast<float>(mouse.GetPositionY()) };
        if (Input::Instance().GetGamePad().GetUseKeybord())cursorSprite->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64);
        // UI
        Bar->render(dc, 0, 0, 600, 300, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 208, 105, 344 * w, 78, 1.0f, 1.0f, 1.0f, 1.0f, 0);

        // Goal
        Goal::Instance().Render(dc);

        // ���j���[
        Menu::Instance().Render(elapsedTime);

        // �t�F�[�h�p
        Fade::Instance().Render(elapsedTime);

    }


#ifdef USE_IMGUI   

    ImGui::Begin("ImGUI");          

    ImGui::SliderFloat("gaussian_sigma", &sigma, 0, 2);
    ImGui::SliderFloat("bloom_intensity", &intensity, 0, 0.5f);
    ImGui::SliderFloat("expo", &exp, 0, 10);
    ImGui::SliderFloat("LEx", &LErgb.x, 0, 1);
    ImGui::SliderFloat("LEy", &LErgb.y, 0, 1);
    ImGui::SliderFloat("LEz", &LErgb.z, 0, 1);


    ImGui::End();   
#endif

}

// player�����񂾂Ƃ� ���̃��Z�b�g�p
void SceneTutorial::Reset()
{
    // �v���C���[�h�� �|�W�V�������Z�b�g    
    player->Init();
    player->SetPosition(Vec3(-19, 35.0f, 0));    
  
    // �S�[���s��
    Goal::Instance().SetCanGoal(false);

    // ���܂Ȃ�
    EnemyBulletManager::Instance().Clear();
    SBManager::Instance().Clear();
    // �G�h�� �|�W�V�������Z�b�g
    EnemyManager::Instance().EnemyReset();  
    EnemyManager::Instance().Init();

    // �ϐ�������
    isTutorial = true;
    isPause = false;
    camTargetPos = { -19,5,0 };
    cameraTargetChange = false;
    isSlow = false;

    radian = 0.0f;
    tutorialTick = 0;	// �A�j���[�V�����p�`�b�N	
    stickAnim = 0;	// �A�j���[�V����    

    renderSlow = false;
    renderAttack = false;
    renderMove = false;
    renderJump = false;
    renderSB = false;


    // �ŏ��̓v���C���[����s�� �X���[���͂��Ēe���Ԃ��Ă��瓮����   
    player->SetIsControl(false);
    player->SetCanSlow(false);
    player->SetCanAttack(false);
    player->SetIsTutorial(isTutorial);
}

// �G�̏�����
void SceneTutorial::EnemyInitialize(ID3D11Device* device)
{
    for (int i = 0; i < ENEMY_MAX; i++)
    {
        if (ENEMY_MAX == i)
        {
            // ���[�h���X�V
            AddLoadPercent(1.0f);
        }

        // �ߐ�
        if (i == 1)
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
void SceneTutorial::EnemyStatusSetting()
{

    enemyPos[0] = { -2.0f,0.5f };
    enemyPos[1] = { -70.0f,0.5f };
    enemyPos[2] = { -150.0f,10.5f };

    enemyGroup[0] = 0;
    enemyGroup[1] = 1;
    enemyGroup[2] = 2;

    enemyWalk[0] = false;
    enemyWalk[1] = false;
    enemyWalk[2] = false;
    
    enemyDirection[0] = false;
    enemyDirection[1] = true;
    enemyDirection[2] = true;

}


// �G�l�~�[�U���\���`��
void SceneTutorial::RenderEnemyAttack()
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

// �`���[�g���A���摜�`��
void SceneTutorial::RenderTutorial()
{
    // �`�b�N
    float tutorialTick2 = 0.0f;
    int oldTick = tutorialTick;

    radian += 0.1f;
    if (radian > 3.1415f) radian = 0.0f;
    // �A�j���[�V�����p�P���O��
    tick = sin(radian);
    if (tick >= 0.5f)
    {
        tutorialTick = 1;
        tutorialTick2 = 0;        
    }
    else
    {
        tutorialTick = 0;
        tutorialTick2 = 1;       
    }
    if (oldTick != tutorialTick)
    {
        stickAnim++;
        if (stickAnim >= 4)stickAnim = 0;
    }

    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
    // �r���[�|�[�g
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);
    // �ϊ��s��
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
    // �v���C���[�̓���ɕ\��   
    DirectX::XMFLOAT3 worldPosition = player->GetCenterPosition();
    worldPosition.y += player->GetHeight();
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

    // ��������̉摜�`��

    if (renderSlow)
    {
        if (isKeybord) KeySHIFT->render(dc, screenPosition.x - 64, screenPosition.y - 32, 128, 64, 1, 1, 1, 1, 0, static_cast<float>(400 * tutorialTick), 0, 400, 200);
        else ButtonLT->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick), 0, 200, 200);
    }
    if (renderAttack)
    {
        if (isKeybord) LeftClick->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick), 0, 200, 200);
        else
        {
            Stick->render(dc, screenPosition.x - 64, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * 3), 0, 200, 200);
            ButtonX->render(dc, screenPosition.x, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick), 0, 200, 200);
        }
    }
    if (renderMove)
    {
        if (isKeybord) {
            KeyA->render(dc, screenPosition.x - 64, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick), 0, 200, 200);
            KeyD->render(dc, screenPosition.x, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick2), 0, 200, 200);
        }
        else Stick->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * stickAnim), 0, 200, 200);
    }
    if (renderJump)
    {
        if (isKeybord) KeySPACE->render(dc, screenPosition.x - 64, screenPosition.y - 32, 128, 64, 1, 1, 1, 1, 0, static_cast<float>(400 * tutorialTick), 0, 400, 200);
        else ButtonA->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick), 0, 200, 200);
    }
    if(renderSB)
    {
        if (isKeybord) RightClick->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick), 0, 200, 200);
        else
        {
            Stick->render(dc, screenPosition.x - 64, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * 1), 0, 200, 200);
            ButtonRT->render(dc, screenPosition.x, screenPosition.y - 32, 64, 64, 1, 1, 1, 1, 0, static_cast<float>(200 * tutorialTick), 0, 200, 200);
        }
    }

}
