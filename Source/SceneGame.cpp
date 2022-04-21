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
#include "StageCollision.h"

#include "Framework.h"

#include"EffectManager.h"


// 初期化
void SceneGame::Initialize()
{
    // ロード％初期化    
    SetLoadPercent(0.0f);

    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();


    // プレイヤー
    player = std::make_unique<Player>(device);
    player->Init(); 

    // ロード％更新
    AddLoadPercent(1.0f);

    // ステージ
    {
        StageManager::Create();
        StageManager::Instance().Init();

        // ロード％更新
        AddLoadPercent(1.0f);

        StageMain* stageMain = new StageMain(device);
        stageMain->PlayerData(player.get());
        StageManager::Instance().Register(stageMain);
        StageCollision* stageCollision = new StageCollision(device);
        StageManager::Instance().Register(stageCollision);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    }
    // ロード％更新
    AddLoadPercent(1.0f);

    // エネミー座標設定
    EnemyPositionSetting();

    // エネミー初期化			
    int ENEMY_MAX = 9;
    for (int i = 0; i <ENEMY_MAX; i++)
    {        
        if (ENEMY_MAX / 2 == i)
        {
            // ロード％更新
            AddLoadPercent(1.0f);
        }
        if (ENEMY_MAX == i)
        {
            // ロード％更新
            AddLoadPercent(2.0f);            
        }

         EnemyGunner* gunner = new EnemyGunner(device);
         gunner->SetPosition(DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0));

        if (i < 4)gunner->SetWalkFlag(true); //歩き回るかどうか
        else gunner->SetWalkFlag(false);

        EnemyManager::Instance().Register(gunner);
        EnemyManager::Instance().Init();
    }

    // ロード％更新
    AddLoadPercent(1.0f);

    // マウスカーソル動かすかどうか
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
    Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");
    gameStart = new Sprite(device, L"./Data/Sprites/scene//start.png");
    gameEnd = new Sprite(device, L"./Data/Sprites/scene//end.png");
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");

    start = true;
    end = false;

    SetLoadPercent(10.0f);
}

// 終了化
void SceneGame::Finalize()
{
    // エネミー終了処理	
    EnemyManager::Instance().Clear();
    // ステージ終了処理
    StageManager::Instance().Clear();
    StageManager::Destory();    

    delete gameStart;
    delete gameEnd;

    // カメラシェイク（簡素）
    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.SetShakeFlag(false);

    delete enemyattack;
    delete LoadBar;
    delete Bar;    
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();   

    if (menuflag == false)
    {

        float slowElapsedTime = elapsedTime * player->GetPlaybackSpeed();
        //TODO: 敵の数増えるとelapsedTime　おかしくなる
        // ヒットストップ
        slowElapsedTime = slowElapsedTime * player->GetHitStopSpeed();
        // スローモーション
        slowElapsedTime = slowElapsedTime * player->GetPlaybackSpeed();



        DirectX::XMFLOAT3 screenPosition;
        screenPosition.x = static_cast<float>(mouse.GetPositionX());
        screenPosition.y = static_cast<float>(mouse.GetPositionY());


        // ステージ
        StageManager::Instance().Update(slowElapsedTime);


        // プレイヤー
        {
            player->Update(slowElapsedTime);
            // シフトブレイク更新処理
            SBManager::Instance().Update(slowElapsedTime);
        }

        // カメラ
        {
            CameraManager& cameraMgr = CameraManager::Instance();
            
            cameraMgr.Update(slowElapsedTime);

            Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
            CameraManager::Instance().SetTarget(target);
        }


        // エネミー
        {
            EnemyManager::Instance().SetPlayer(player.get());
            // ソート
            EnemyManager::Instance().SortLengthSq(player->GetPosition());
            // エネミー更新処理
            EnemyManager::Instance().Update(slowElapsedTime);
            // 弾丸更新処理
            EnemyBulletManager::Instance().Update(slowElapsedTime);

        }

        //エフェクト更新処理
        EffectManager::Instance().Update(slowElapsedTime);
      

        // スロー時間表示
        w = player->GetSlowTimer() / player->GetSlowMax();
        et = elapsedTime;
    }
    //ポーズメニュー
    if (gamePad.GetButtonDown() & GamePad::BTN_START)
    {
        if (!menuflag) menuflag = true;
        else menuflag = false;
    }
    if (menuflag == true)
    {
        menu();
        SceneSelect();
    }


    // リセット
    if (player->GetHealth() <= 0)// ||  gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        // デバッグ用で消してる
        Reset();
    }



    // TODO 現在のステージの死んでるエネミーの数が０の場合 次のステージへ
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        // 次のステージへ移る処理
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneClear));
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
        Bar->render(dc, 600, 650, 620, 25, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 605, 652, 605 * w, 21, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        if (menuflag == true)
        {
            gameStart->render(dc, startpos.x, startpos.y, startsize.x, startsize.y, 1, 1, 1, startAlpha, 0);
            gameEnd->render(dc, endpos.x, endpos.y, endsize.x, endsize.y, 1, 1, 1, endAlpha, 0);
        }
        RenderEnemyAttack();
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

// playerが死んだとき 等のリセット用
void SceneGame::Reset()
{
    // たまなし
    EnemyBulletManager::Instance().Clear();
    SBManager::Instance().Clear();
    // 敵蘇生 ポジションリセット
    EnemyManager::Instance().Init();
    for (int i = 0; i < EnemyManager::Instance().GetEnemyCount(); i++)
    {
        EnemyManager::Instance().SetPosition(i, DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0));
    }

    // プレイヤー蘇生 ポジションリセット
    player->Init();

}

// エネミー座標設定
void SceneGame::EnemyPositionSetting()
{

    enemyPos[0] = {-75,0.5f};
    enemyPos[1] = {-85,0.5f};
    enemyPos[2] = {-105,0.5f};
    enemyPos[3] = {-120,0.5f};
    enemyPos[4] = {-40,20.5f};
    enemyPos[5] = {4,42.5f};
    enemyPos[6] = {-130,85.5f};
    enemyPos[7] = {-65,55.5f};
    enemyPos[8] = {20,70.5f};    
}

//メニュー
void SceneGame::menu()
{
    Mouse& mouse = Input::Instance().GetMouse();
    DirectX::XMFLOAT3 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetPositionY());
    mousepos.x = screenPosition.x;
    mousepos.y = screenPosition.y;

    //マウス左クリックでマップ選択
    const mouseButton mouseClick =
        Mouse::BTN_LEFT;

    GamePad& gamePad = Input::Instance().GetGamePad();


    if (start && (mouse.GetButtonDown() & mouseClick))
    {
       // SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
    }
    else if (end && (mouse.GetButtonDown() & mouseClick))
    {
        DestroyWindow(GetActiveWindow());
    }
    
    const GamePadButton up =
        GamePad::BTN_UP;
        /*| GamePad::BTN_DOWN
        | GamePad::BTN_W
        | GamePad::BTN_S;*/
    const GamePadButton down =
        GamePad::BTN_DOWN;

    if (screenPosition.x >= startpos.x && screenPosition.x < startpos.x + startsize.x)
    {
        if (screenPosition.y >= startpos.y && screenPosition.y <= startpos.y + startsize.y)
        {
            start = true;
            end = false;
        }
        else if (screenPosition.y >= endpos.y && screenPosition.y <= endpos.y + endsize.y)
        {
            start = false;
            end = true;
        }
    }

    if (gamePad.GetButtonDown() & up)
    {
        start = true;
        end = false;
    }
    else if (gamePad.GetButton() & down)
    {
        start = false;
        end = true;
    }

}


void SceneGame::SceneSelect()
{

    if (start)
    {
        startAlpha = 1.0f;
        endAlpha = 0.4f;
    }
    if (end)
    {
        startAlpha = 0.4f;
        endAlpha = 1.0f;
    }
}


// エネミー攻撃予兆描画
void SceneGame::RenderEnemyAttack()
{
    
    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();

    // ビューポート
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);
    // 変換行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
    // 全ての敵の頭上にHPゲージを表示
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        DirectX::XMFLOAT3 worldPosition = enemy->GetCenterPosition();
        worldPosition.y += enemy->GetHeight();
        DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
        // ワールド座標からスクリーン座標へ変換
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
        // スクリーン座標
        DirectX::XMFLOAT2 screenPosition;
        DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);                

        if (enemy->GetIsAttack() && enemy->GetHealth() > 0)
        {
            // 攻撃予兆描画
            enemyattack->render(dc, screenPosition.x - 32, screenPosition.y - 64, 64, 64, 1, 1, 1, 1, 0);
        }
    }   
}