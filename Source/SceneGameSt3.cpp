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


// 初期化
void SceneGameSt3::Initialize()
{
    // ロード％初期化    
    SetLoadPercent(0.0f);
    BGM = Audio::Instance().LoadAudioSource("Data\\Audio\\BGM\\GameBGM3.wav", true);
    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();
    Graphics& gfx = Graphics::Ins();

    // プレイヤー
    player = std::make_unique<Player>(device);
    player->Init();
    player->SetSt3(true);
    player->SetPosition(Vec3(40.0f, 0.5f, 0.0f));

    // ロード％更新
    AddLoadPercent(1.0f);

    // ステージ

    StageManager::Create();
    StageManager::Instance().Init();

    // ロード％更新
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

    // ドア
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
    // ロード％更新
    AddLoadPercent(1.0f);

    // エネミー
    {
        // エネミー座標設定
        EnemyStatusSetting();
        // エネミー初期化			    
        EnemyInitialize(device);
    }

    // ロード％更新
    AddLoadPercent(1.0f);

    // マウスカーソル動かすかどうか
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

    // ロード％更新
    AddLoadPercent(1.0f);

    Fade::Instance().Initialize();

    Goal::Instance().Init(device);
    Goal::Instance().SetGoalPos(stageMain->GetGoalPos());


    //デバッグ
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

    // ロード％ 100%
    SetLoadPercent(10.0f);

    // 変数初期化
    changeScene = false;
    ResetPos = { 40,0.5,0 };
    checkPoint = false;
}

// 終了化
void SceneGameSt3::Finalize()
{
    // エネミー終了処理	
    EnemyManager::Instance().Clear();

    // ドア終了処理
    DoorManager::Instance().Clear();

    // ステージ終了処理
    StageManager::Instance().Clear();
    StageManager::Destory();

    Menu::Instance().Finalize();

    // カメラシェイク（簡素）
    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.SetShakeFlag(false);

    delete fade;
    delete enemyattack;
    delete LoadBar;
    delete Bar;
    delete cursorSprite;
    //デバッグ
   // delete hitEffect;
}

// 更新処理
void SceneGameSt3::Update(float elapsedTime)
{
    BGM.get()->Play(0.6f);
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    if (Menu::Instance().GetMenuFlag() == false)
    {

        float slowElapsedTime = elapsedTime * player->GetPlaybackSpeed();
        ////TODO: 敵の数増えるとelapsedTime　おかしくなる
        //// ヒットストップ
        //slowElapsedTime = slowElapsedTime * player->GetHitStopSpeed();
        //// スローモーション
        //slowElapsedTime = slowElapsedTime * player->GetPlaybackSpeed();



        DirectX::XMFLOAT3 screenPosition;
        screenPosition.x = static_cast<float>(mouse.GetPositionX());
        screenPosition.y = static_cast<float>(mouse.GetPositionY());


        // ステージ
        StageManager::Instance().Update(slowElapsedTime);

        // ドア
        DoorManager::Instance().Update(elapsedTime);

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
            CameraManager::Instance().SetGoal(target);
        }


        // エネミー
        if (!player->GetClock())
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

    // リセット座標更新
    UpdateResetPos();

    // リセット
    if (player->GetReset()) {
        // フェードアウト
        if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);

        // フェードアウトおわったら
        if (Fade::Instance().GetFadeOutFinish()) {
            // リセット
            Reset();

            // フェードイン
            Fade::Instance().SetFadeInFlag(true);
        }
    }
    // Menu
    Menu::Instance().Update(elapsedTime);
    // Fade
    if (Menu::Instance().GetChangeFlag())
    {
        // フェードアウト
        if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);

        // フェードアウトおわったら
        if (Fade::Instance().GetFadeOutFinish()) {

            // リセット座標をリセット
            ResetPos = { 40,0.5,0 };
            checkPoint = false;

            // リセット
            Reset();

            // フェードイン
            Fade::Instance().SetFadeInFlag(true);
        }
    }
    // フェードイン終わったら初期化
    if (Fade::Instance().GetFadeInFinish()) Fade::Instance().Initialize();

    // Fade
    Fade::Instance().Update(elapsedTime);
    // Goal
    Goal::Instance().SetPlayerPos(player->GetCenterPosition());
    Goal::Instance().Update(elapsedTime);

    // 現在のステージの死んでるエネミーの数が０の場合
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        Goal::Instance().SetCanGoal(true);
        // ゴールと判定とる         
        if (StageManager::Instance().CollisionPlayerVsNextStagePos(player->GetCenterPosition(), player->GetRadius()))
        {
            changeScene = true;
            // フェードアウト
            if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);
        }
    }
    // フェードアウトおわったら次のシーンにいける
    if (changeScene && Fade::Instance().GetFadeOutFinish())
    {
        BGM.get()->Stop();
        // 次のステージへ移る処理
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGameSt4));
    }
    
}

// 描画処理
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

    // 通常レンダリング
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //DirectX::XMFLOAT4X4 data{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    //// TODO:05 Bind the transformation matrix data to the vertex shader at register number 0.
    //dc->UpdateSubresource(constant_buffer, 0, 0, &data, 0, 0);
    //dc->VSSetConstantBuffers(3, 1, &constant_buffer);

    framebuffer[0]->clear(dc, 1.0f, 1.0f, 1.0f, 0.0f);
    framebuffer[0]->activate(dc);
    {
        // モデル描画
        {
            // ステージ描画
            StageManager::Instance().Render(dc, elapsedTime);
            // スロー演出、敵や自機など重要なオブジェクト以外を暗くする
            fade->render(dc, 0, 0, 1920, 1080, 1, 1, 1, player->GetSlowAlpha(), 0);
            // ドア描画
            DoorManager::Instance().Render(dc, elapsedTime);
            // プレイヤー描画
            player->Render(dc);
            // エネミー描画
            EnemyManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader(), player->GetSlowFlag() & !player->GetDead());
            EnemyBulletManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader(), player->GetSlowFlag() & !player->GetDead());
        }

        // デバック
        {
            player->DrawDebugGUI();
        }

        //3Dエフェクト描画
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


    // 2D描画
    {
        // 攻撃予兆描画
        RenderEnemyAttack();

        Mouse& mouse = Input::Instance().GetMouse();
        GamePad& pad = Input::Instance().GetGamePad();
        if (Input::Instance().GetGamePad().GetUseKeybord())
        {
            // マウス位置
            Vec2 screenPosition = { static_cast<float>(mouse.GetPositionX()) ,static_cast<float>(mouse.GetPositionY()) };
            cursorSprite->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64);
        }
        else
        {
            // ワープストライク投げる向きを設定
            Vec3 WSdirection = { -pad.GetAxisLX(), pad.GetAxisLY(), 0 };

            // ビューポート
            D3D11_VIEWPORT viewport;
            UINT numViewports = 1;
            dc->RSGetViewports(&numViewports, &viewport);
            // 変換行列
            DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
            DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
            DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
            DirectX::XMFLOAT3 worldPosition = player->GetCenterPosition() + (WSdirection * 20.0f);
            DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
            // ワールド座標からスクリーン座標へ変換
            DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(WorldPosition,
                viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth, Projection, View, World);
            // スクリーン座標
            DirectX::XMFLOAT2 screenPosition;
            DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

            if (WSdirection.x != 0 && WSdirection.y != 0)cursorSprite->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64);
        }
        // UI
        Bar->render(dc, 400, 500, 500, 225, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 570, 578.75f, 290 * w, 58.5f, 1.0f, 1.0f, 1.0f, 0.8f, 0);

        // Goal
        Goal::Instance().Render(dc);

        // メニュー
        Menu::Instance().Render(elapsedTime);

        // フェード用
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

// playerが死んだとき 等のリセット用
void SceneGameSt3::Reset()
{
    // ゴール不可
    Goal::Instance().SetCanGoal(false);

    // たまなし
    EnemyBulletManager::Instance().Clear();
    SBManager::Instance().Clear();
    // 敵蘇生 ポジションリセット
    EnemyManager::Instance().Init();
    EnemyManager::Instance().EnemyReset();
    // ドアリセット
    DoorManager::Instance().Init();

    // プレイヤー蘇生 ポジションリセット
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

// 敵の初期化
void SceneGameSt3::EnemyInitialize(ID3D11Device* device)
{
    bool melee, gunner, shotgunner;

    for (int i = 0; i < ENEMY_MAX; i++)
    {
        if (ENEMY_MAX == i)
        {
            // ロード％更新
            AddLoadPercent(1.0f);
        }
                
        // 近接 01347
        if (i == 0 || i == 1 || i == 3 || i == 5 || i == 7 || i == 10)
        {
            melee = true;
            gunner = shotgunner = false;
        }
        // 遠隔 289
        else if (i == 2 || i == 8 || i == 9)
        {
            gunner = true;
            melee = shotgunner = false;
        }
        // 遠隔(散) 56
        else if (i == 4 || i == 6 )
        {
            shotgunner = true;
            gunner = melee = false;
        }        

        // 近接
        if (melee)
        {
            EnemyMelee* melee = new EnemyMelee(device);

            // 座標セット
            melee->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            melee->PositionInitialize();
            //歩き回るかどうか
            melee->SetInitialWalk(enemyWalk[i]);
            melee->WalkFlagInitialize();
            // グループ番号セット
            melee->SetInitialGroupNum(enemyGroup[i]);
            melee->GroupNumInitialize();
            // 向きセット
            melee->SetInitialDirection(enemyDirection[i]);
            melee->DirectionInitialize();

            EnemyManager::Instance().Register(melee);
        }
        else if (shotgunner)
        {
            EnemyShotGunner* gunner = new EnemyShotGunner(device);

            // 座標セット
            gunner->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            gunner->PositionInitialize();
            //歩き回るかどうか
            gunner->SetInitialWalk(enemyWalk[i]);
            gunner->WalkFlagInitialize();
            // グループ番号セット
            gunner->SetInitialGroupNum(enemyGroup[i]);
            gunner->GroupNumInitialize();
            // 向きセット
            gunner->SetInitialDirection(enemyDirection[i]);
            gunner->DirectionInitialize();

            EnemyManager::Instance().Register(gunner);
        }
        else if (gunner)
        {
            EnemyGunner* gunner = new EnemyGunner(device);

            // 座標セット
            gunner->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            gunner->PositionInitialize();
            //歩き回るかどうか
            gunner->SetInitialWalk(enemyWalk[i]);
            gunner->WalkFlagInitialize();
            // グループ番号セット
            gunner->SetInitialGroupNum(enemyGroup[i]);
            gunner->GroupNumInitialize();
            // 向きセット
            gunner->SetInitialDirection(enemyDirection[i]);
            gunner->DirectionInitialize();

            EnemyManager::Instance().Register(gunner);
        }

    }

    EnemyManager::Instance().Init();
}

// エネミー座標設定
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


// エネミー攻撃予兆描画
void SceneGameSt3::RenderEnemyAttack()
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
        // スクリーン座3
        DirectX::XMFLOAT2 screenPosition;
        DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

        if (enemy->GetIsSearch() && !enemy->GetIsAttack() && enemy->GetHealth() > 0)
        {
            // 攻撃予兆描画
            enemyattack->render(dc, screenPosition.x - 32, screenPosition.y - 64, 64, 64, 1, 1, 1, 1, 0);
        }
    }
}

// リセット座標更新
void SceneGameSt3::UpdateResetPos()
{
    // プレイヤーの座標によってリセットしたときの座標変える  
    Vec3 collision = { -125.0f,30.0f,0.0f };
    // スフィアでコリジョンとる
    if (Collision::SphereVsSphere(collision, player->GetCenterPosition(), 6.0f, player->GetRadius()))
    {
        // 壊れた壁のとこ入ったら更新する
        ResetPos = { -125.0f,37.0f,0 };
        checkPoint = true;
    }

    // この時点で死んでるエネミーは死んだままにする 
}
