
#include "SceneTutorial.h"

#include "Input/Input.h"

#include "Camera/CameraManager.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"


#include "SceneLoading.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Menu.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain0.h"
#include "StageCollision0.h"

#include "Framework.h"

#include"EffectManager.h"


// ������
void SceneTutorial::Initialize()
{
    // ���[�h��������    
    SetLoadPercent(0.0f);

    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();


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

        StageMain0* stageMain = new StageMain0(device);
        stageMain->PlayerData(player.get());
        StageManager::Instance().Register(stageMain);
        StageCollision0* stageCollision = new StageCollision0(device);
        StageManager::Instance().Register(stageCollision);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    }
    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �G�l�~�[���W�ݒ�
    EnemyPositionSetting();

    // �G�l�~�[������			    
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
            AddLoadPercent(2.0f);
        }

        EnemyGunner* gunner = new EnemyGunner(device);
        gunner->SetPosition(DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0));

        gunner->SetWalkFlag(false); //������邩�ǂ���        

        EnemyManager::Instance().Register(gunner);
        EnemyManager::Instance().Init();
    }

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �}�E�X�J�[�\�����������ǂ���
    Input::Instance().GetMouse().SetMoveCursor(true);
  
    // �X�v���C�g
    Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");

    // ���j���[
    Menu::Instance().Initialize();

    // ���[�h�� 100%
    SetLoadPercent(10.0f);
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

    delete enemyattack;
    delete LoadBar;
    delete Bar;
}

// �X�V����
void SceneTutorial::Update(float elapsedTime)
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
    if (player->GetHealth() <= 0)// ||  gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        // �f�o�b�O�p�ŏ����Ă�
        Reset();
    }


    // Menu
    Menu::Instance().Update(elapsedTime);


    // TODO ���݂̃X�e�[�W�̎���ł�G�l�~�[�̐����O�̏ꍇ ���̃X�e�[�W��
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        // ���̃X�e�[�W�ֈڂ鏈��
        // �X�e�[�W�P��
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

    // 2D�`��
    {
        Bar->render(dc, 600, 650, 620, 25, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 605, 652, 605 * w, 21, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        Menu::Instance().Render(elapsedTime);
        RenderEnemyAttack();
    }


#ifdef USE_IMGUI   


    ImGui::Begin("ImGUI");
 
    ImGui::SliderFloat("elapsedTime", &et, 0.0f, 1.0f);

    bool sh = cameraMgr.GetShakeFlag();
    ImGui::Checkbox("shakeFlag", &sh);

    ImGui::End();  
#endif

}

// player�����񂾂Ƃ� ���̃��Z�b�g�p
void SceneTutorial::Reset()
{
    // ���܂Ȃ�
    EnemyBulletManager::Instance().Clear();
    // �G�h�� �|�W�V�������Z�b�g
    EnemyManager::Instance().Init();
    int group = 0;
    bool walk = false;
    for (int i = 0; i < EnemyManager::Instance().GetEnemyCount(); i++)
    {
        //������邩�ǂ���
        if (i < 4)walk = true;
        else walk = false;

        // �O���[�v�ԍ��Z�b�g
        if (i < 4)group = 0;
        else if (i < 6)group = 1;
        else if (i < 9)group = 2;

        EnemyManager::Instance().SetPosition(i, DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0), group, walk);
    }
    // �v���C���[�h�� �|�W�V�������Z�b�g
    player->Init();

}

// �G�l�~�[���W�ݒ�
void SceneTutorial::EnemyPositionSetting()
{
    enemyPos[0] = { 0.0f,0.5f };
    enemyPos[1] = { 0.0f,0.5f };
    enemyPos[2] = { 0.0f,0.5f };   
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