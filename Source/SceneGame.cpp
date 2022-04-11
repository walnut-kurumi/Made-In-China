#include "SceneGame.h"
#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain.h"

#include "Framework.h"

#include"EffectManager.h"


// ������
void SceneGame::Initialize()
{
    HRESULT hr{ S_OK };
    // ���[�h��������    
    AddLoadPercent(0.0f);

    ID3D11Device* device = Graphics::Ins().GetDevice();    

    StageManager::Create();
    StageManager::Instance().Init();    

    // ���[�h���X�V
    AddLoadPercent(24.0f);    

    StageMain* stageMain = new StageMain(device);
    StageManager::Instance().Register(stageMain);
    StageSkybox* skybox = new StageSkybox(device);
    StageManager::Instance().Register(skybox);

    // ���[�h���X�V
    AddLoadPercent(24.0f);

    player = new Player(device);
    player->Init(); 

    // ���[�h���X�V
    AddLoadPercent(24.0f);

    // �G�l�~�[���W�ݒ�
    EnemyPositionSetting();

    // �G�l�~�[������			
    for (int i = 0; i <4; i++)
    {
        if (i == 3)
        {
            // ���[�h�� 100%
            SetLoadPercent(122.0f);
        }
        else
        {
            // ���[�h���X�V
            AddLoadPercent(2.0f);
        }
               EnemyGunner* gunner = new EnemyGunner(device);
        gunner->SetPosition(DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0));
        EnemyManager::Instance().Register(gunner);
        EnemyManager::Instance().Init();

        
    }

    // �}�E�X�J�[�\����������
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
    SetLoadPercent(122.0f);
}

// �I����
void SceneGame::Finalize()
{
    // �G�l�~�[�I������	
    EnemyManager::Instance().Clear();

    StageManager::Instance().Clear();
    StageManager::Destory();    
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    elapsedTime = elapsedTime * player->GetPlaybackSpeed();

    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    //�}�E�X���N���b�N
    const mouseButton mouseClick =
        Mouse::BTN_LEFT;

    // �Ȃɂ��{�^������������Q�[���V�[���؂�ւ�
    const GamePadButton anyButton =
        /*GamePad::BTN_A
        | */GamePad::BTN_B
        | GamePad::BTN_BACK
        | GamePad::BTN_DOWN
        | GamePad::BTN_LEFT
        | GamePad::BTN_LEFT_SHOULDER
        | GamePad::BTN_LEFT_THUMB
        //| GamePad::BTN_LEFT_TRIGGER
        | GamePad::BTN_RIGHT
        | GamePad::BTN_RIGHT_SHOULDER
        | GamePad::BTN_RIGHT_THUMB
        //| GamePad::BTN_RIGHT_TRIGGER
        | GamePad::BTN_START
        | GamePad::BTN_UP
        //| GamePad::BTN_X
        | GamePad::BTN_Y;

    DirectX::XMFLOAT3 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetPositionY());

    StageManager::Instance().Update(elapsedTime);



    player->Update(elapsedTime);
    // �V�t�g�u���C�N�X�V����
    SBManager::Instance().Update(elapsedTime);

    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.Update(elapsedTime);

    Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
    CameraManager::Instance().SetTarget(target);

    // �G�l�~�[
    {
        EnemyManager::Instance().SetPlayer(player);
        // �\�[�g
        EnemyManager::Instance().SortLengthSq(player->GetPosition());
        // �G�l�~�[�X�V����
        EnemyManager::Instance().Update(elapsedTime);
        // �e�ۍX�V����
        EnemyBulletManager::Instance().Update(elapsedTime);

    }
    
    //�G�t�F�N�g�X�V����
    EffectManager::Instance().Update(elapsedTime);

    // TODO
    // ���݂̃X�e�[�W�̃G�l�~�[�̐����O�̏ꍇ ���̃X�e�[�W��
    if (EnemyManager::Instance().GetEnemyCount() <= 0)
    {
        // ���̃X�e�[�W�ֈڂ鏈��
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
    }


#ifdef USE_IMGUI   


    ImGui::Begin("ImGUI");

    // CAMERA_SHAKE
    // TODO:07 Adjust the maximum amount of rotation(max_skew) and movement(max_sway) of the camera.
    ImGui::SliderFloat("max_sway [pixel]", &max_sway, 0.0f, 64.0f);
    ImGui::SliderFloat("max_skew [degree]", &max_skew, 0.0f, 10.0f);
    // TODO:12 Defines the amount of seed shifting factor for perlin noise.
    ImGui::SliderFloat("seed_shifting_factor", &seed_shifting_factor, 0.0f, 10.0f);

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

void SceneGame::Reset()
{
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
