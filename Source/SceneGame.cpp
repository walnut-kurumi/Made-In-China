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

// ������
void SceneGame::Initialize()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();    

    StageManager::Create();
    StageManager::Instance().Init();

    StageMain* stageMain = new StageMain(device);
    StageManager::Instance().Register(stageMain);
    StageSkybox* skybox = new StageSkybox(device);
    StageManager::Instance().Register(skybox);

    player = new Player(device);
    player->Init(); 

    // �G�l�~�[������			
    for (int i = 0; i < 3; i++)
    {
        EnemyGunner* gunner = new EnemyGunner(device);
        gunner->SetPosition(DirectX::XMFLOAT3(i * 5.0f, 0, 0));
        EnemyManager::Instance().Register(gunner);
        EnemyManager::Instance().Init();
    }

    Input::Instance().GetMouse().SetMoveCursor(false);
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
        GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_BACK
        | GamePad::BTN_DOWN
        | GamePad::BTN_LEFT
        | GamePad::BTN_LEFT_SHOULDER
        | GamePad::BTN_LEFT_THUMB
        | GamePad::BTN_LEFT_TRIGGER
        | GamePad::BTN_RIGHT
        | GamePad::BTN_RIGHT_SHOULDER
        | GamePad::BTN_RIGHT_THUMB
        | GamePad::BTN_RIGHT_TRIGGER
        | GamePad::BTN_START
        | GamePad::BTN_UP
        | GamePad::BTN_X
        | GamePad::BTN_Y;

    DirectX::XMFLOAT3 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetPositionY());

    StageManager::Instance().Update(elapsedTime);



    player->Update(elapsedTime);

    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.Update(elapsedTime);

    Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
    CameraManager::Instance().SetTarget(target);

    // �G�l�~�[�X�V����
    EnemyManager::Instance().Update(elapsedTime);
    // �\�[�g
    EnemyManager::Instance().SortLengthSq(player->GetPosition());
}

// �`�揈��
void SceneGame::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();    

 
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
    
    // 2D�`��
    {
    }


#ifdef USE_IMGUI   

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