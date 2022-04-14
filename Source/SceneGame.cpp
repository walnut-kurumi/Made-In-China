#include "SceneGame.h"
#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"

#include"SceneLoading.h"
#include"SceneClear.h"
#include"SceneOver.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain.h"

#include "Framework.h"

#include"EffectManager.h"


// ������
void SceneGame::Initialize()
{
    // ���[�h��������    
    SetLoadPercent(0.0f);

    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();

    // �X�e�[�W
    {
        StageManager::Create();
        StageManager::Instance().Init();

        // ���[�h���X�V
        AddLoadPercent(1.0f);

        StageMain* stageMain = new StageMain(device);
        StageManager::Instance().Register(stageMain);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    }

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �v���C���[
    player = new Player(device);
    player->Init(); 

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �G�l�~�[���W�ݒ�
    EnemyPositionSetting();

    // �G�l�~�[������			
    int ENEMY_MAX = 9;
    for (int i = 0; i <ENEMY_MAX; i++)
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

    // CAMERA_SHAKE
    // TODO:02 Create a constant buffer object.
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, &constant_buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // ���[�h�� 100%
    Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");
    SetLoadPercent(10.0f);
}

// �I����
void SceneGame::Finalize()
{
    // �G�l�~�[�I������	
    EnemyManager::Instance().Clear();
    // �X�e�[�W�I������
    StageManager::Instance().Clear();
    StageManager::Destory();    
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();   

    if (menuflag == false)
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
        {
            EnemyManager::Instance().SetPlayer(player);
            // �\�[�g
            EnemyManager::Instance().SortLengthSq(player->GetPosition());
            // �G�l�~�[�X�V����
            EnemyManager::Instance().Update(slowElapsedTime);
            // �e�ۍX�V����
            EnemyBulletManager::Instance().Update(slowElapsedTime);

        }

        //�G�t�F�N�g�X�V����
        EffectManager::Instance().Update(slowElapsedTime);


        // TODO ���݂̃X�e�[�W�̎���ł�G�l�~�[�̐����O�̏ꍇ ���̃X�e�[�W��
        if (EnemyManager::Instance().GetDeadEnemyCount() <= 0)
        {
            // ���̃X�e�[�W�ֈڂ鏈��
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneClear));
        }


        // �X���[���ԕ\��
        w = player->GetSlowTimer() / slowMaxTime;
        et = elapsedTime;
    }
    //�|�[�Y���j���[
    if (gamePad.GetButtonDown() & GamePad::BTN_START)
    {
        if (!menuflag) menuflag = true;
        else menuflag = false;
    }
    if (menuflag == true)
    {
        menu();
    }


    // ���Z�b�g
    if (player->GetHealth() <= 0 ||  gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        // �f�o�b�O�p�ŏ����Ă�
        //Reset();
    }
}

// �`�揈��
void SceneGame::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();    
    CameraManager& cameraMgr = CameraManager::Instance();

    DirectX::XMFLOAT4X4 data{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    // TODO:13 Calculate the amount of camera shake that feels natural using purlin noise.
    seed += elapsedTime;
    const float shake = 2.0f * static_cast<float>(pn.noise(seed * seed_shifting_factor, seed * seed_shifting_factor, 0)) - 1.0f;

    // TODO:10 Calculate the transformation matrix considering the amount of camera shake.
    XMStoreFloat4x4(&data,
        XMMatrixTranslation(shake * max_sway / 1280, shake * max_sway / 720, 0) *
        XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(shake * max_skew))
    );

    // TODO:05 Bind the transformation matrix data to the vertex shader at register number 0.
    dc->UpdateSubresource(constant_buffer, 0, 0, &data, 0, 0);
    dc->VSSetConstantBuffers(3, 1, &constant_buffer);
 
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
    // �G�h�� �|�W�V�������Z�b�g
    EnemyManager::Instance().Init();
    for (int i = 0; i < EnemyManager::Instance().GetEnemyCount(); i++)
    {
        EnemyManager::Instance().SetPosition(i, DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0));
    }

    // �v���C���[�h�� �|�W�V�������Z�b�g
    player->Init();

}

// �G�l�~�[���W�ݒ�
void SceneGame::EnemyPositionSetting()
{

    enemyPos[0] = {-75,0};
    enemyPos[1] = {-105,0};
    enemyPos[2] = {-140,0};
    enemyPos[3] = {-180,0};
    enemyPos[4] = {-15,35};
    enemyPos[5] = {-100,35};
    enemyPos[6] = {-190,38};
    enemyPos[7] = {-190,62};
    enemyPos[8] = {-10,62};    
}

//���j���[
void SceneGame::menu()
{

}
