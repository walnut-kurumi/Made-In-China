
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


// 初期化
void SceneTutorial::Initialize()
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

        StageMain0* stageMain = new StageMain0(device);
        stageMain->PlayerData(player.get());
        StageManager::Instance().Register(stageMain);
        StageCollision0* stageCollision = new StageCollision0(device);
        StageManager::Instance().Register(stageCollision);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    }
    // ロード％更新
    AddLoadPercent(1.0f);

    // エネミー座標設定
    EnemyPositionSetting();

    // エネミー初期化			    
    for (int i = 0; i < ENEMY_MAX; i++)
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

        gunner->SetWalkFlag(false); //歩き回るかどうか        

        EnemyManager::Instance().Register(gunner);
        EnemyManager::Instance().Init();
    }

    // ロード％更新
    AddLoadPercent(1.0f);

    // マウスカーソル動かすかどうか
    Input::Instance().GetMouse().SetMoveCursor(true);
  
    // スプライト
    Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");

    // メニュー
    Menu::Instance().Initialize();

    // ロード％ 100%
    SetLoadPercent(10.0f);
}

// 終了化
void SceneTutorial::Finalize()
{
    // エネミー終了処理	
    EnemyManager::Instance().Clear();
    // ステージ終了処理
    StageManager::Instance().Clear();
    StageManager::Destory();

    Menu::Instance().Finalize();

    // カメラシェイク（簡素）
    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.SetShakeFlag(false);

    delete enemyattack;
    delete LoadBar;
    delete Bar;
}

// 更新処理
void SceneTutorial::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    if (Menu::Instance().GetMenuFlag() == false)
    {

        float slowElapsedTime = elapsedTime * player->GetPlaybackSpeed();        
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

    // リセット
    if (player->GetHealth() <= 0)// ||  gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        // デバッグ用で消してる
        Reset();
    }


    // Menu
    Menu::Instance().Update(elapsedTime);


    // TODO 現在のステージの死んでるエネミーの数が０の場合 次のステージへ
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        // 次のステージへ移る処理
        // ステージ１へ
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
    }
}

// 描画処理
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

    // 通常レンダリング
    dc->OMSetRenderTargets(1, &rtv, dsv);

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

// playerが死んだとき 等のリセット用
void SceneTutorial::Reset()
{
    // たまなし
    EnemyBulletManager::Instance().Clear();
    // 敵蘇生 ポジションリセット
    EnemyManager::Instance().Init();
    int group = 0;
    bool walk = false;
    for (int i = 0; i < EnemyManager::Instance().GetEnemyCount(); i++)
    {
        //歩き回るかどうか
        if (i < 4)walk = true;
        else walk = false;

        // グループ番号セット
        if (i < 4)group = 0;
        else if (i < 6)group = 1;
        else if (i < 9)group = 2;

        EnemyManager::Instance().SetPosition(i, DirectX::XMFLOAT3(enemyPos[i].x, enemyPos[i].y, 0), group, walk);
    }
    // プレイヤー蘇生 ポジションリセット
    player->Init();

}

// エネミー座標設定
void SceneTutorial::EnemyPositionSetting()
{
    enemyPos[0] = { 0.0f,0.5f };
    enemyPos[1] = { 0.0f,0.5f };
    enemyPos[2] = { 0.0f,0.5f };   
}


// エネミー攻撃予兆描画
void SceneTutorial::RenderEnemyAttack()
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