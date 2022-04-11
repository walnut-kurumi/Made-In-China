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


// 初期化
void SceneGame::Initialize()
{
    HRESULT hr{ S_OK };
    // ロード％初期化    
    AddLoadPercent(0.0f);

    ID3D11Device* device = Graphics::Ins().GetDevice();    

    StageManager::Create();
    StageManager::Instance().Init();    

    // ロード％更新
    AddLoadPercent(24.0f);    

    StageMain* stageMain = new StageMain(device);
    StageManager::Instance().Register(stageMain);
    StageSkybox* skybox = new StageSkybox(device);
    StageManager::Instance().Register(skybox);

    // ロード％更新
    AddLoadPercent(24.0f);

    player = new Player(device);
    player->Init(); 

    // ロード％更新
    AddLoadPercent(24.0f);

    // エネミー座標設定
    EnemyPositionSetting();

    // エネミー初期化			
    for (int i = 0; i <4; i++)
    {
        if (i == 3)
        {
            // ロード％ 100%
            SetLoadPercent(122.0f);
        }
        else
        {
            // ロード％更新
            AddLoadPercent(2.0f);
        }
               EnemyGunner* gunner = new EnemyGunner(device);
        gunner->SetPosition(DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0));
        EnemyManager::Instance().Register(gunner);
        EnemyManager::Instance().Init();

        
    }

    // マウスカーソル動かすか
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

    // ロード％ 100%
    SetLoadPercent(122.0f);
}

// 終了化
void SceneGame::Finalize()
{
    // エネミー終了処理	
    EnemyManager::Instance().Clear();

    StageManager::Instance().Clear();
    StageManager::Destory();    
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    elapsedTime = elapsedTime * player->GetPlaybackSpeed();

    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    //マウス左クリック
    const mouseButton mouseClick =
        Mouse::BTN_LEFT;

    // なにかボタンを押したらゲームシーン切り替え
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
    // シフトブレイク更新処理
    SBManager::Instance().Update(elapsedTime);

    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.Update(elapsedTime);

    Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
    CameraManager::Instance().SetTarget(target);

    // エネミー
    {
        EnemyManager::Instance().SetPlayer(player);
        // ソート
        EnemyManager::Instance().SortLengthSq(player->GetPosition());
        // エネミー更新処理
        EnemyManager::Instance().Update(elapsedTime);
        // 弾丸更新処理
        EnemyBulletManager::Instance().Update(elapsedTime);

    }
    
    //エフェクト更新処理
    EffectManager::Instance().Update(elapsedTime);

    // TODO
    // 現在のステージのエネミーの数が０の場合 次のステージへ
    if (EnemyManager::Instance().GetEnemyCount() <= 0)
    {
        // 次のステージへ移る処理
    }
}

// 描画処理
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
 
    // モデル描画
    {
        // ステージ描画
        StageManager::Instance().Render(dc, elapsedTime);
        // プレイヤー描画
        player->Render(dc);
        // エネミー描画
        EnemyManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());
    }
   
    // デバック
    {
        player->DrawDebugGUI();        
    }
    
    //3Dエフェクト描画
    {
        EffectManager::Instance().Render(cameraMgr.GetView(), cameraMgr.GetProjection());
    }

    // 2D描画
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

// エネミー座標設定
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
