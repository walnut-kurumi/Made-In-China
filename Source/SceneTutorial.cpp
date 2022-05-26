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




// 初期化
void SceneTutorial::Initialize()
{
    // ロード％初期化    
    SetLoadPercent(0.0f);
    BGM = Audio::Instance().LoadAudioSource("Data\\Audio\\BGM\\GameBGM1.wav", true);
    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();
    Graphics& gfx = Graphics::Ins();

    // プレイヤー
    player = std::make_unique<Player>(device);
    // チュートリアルだお
    player->SetIsTutorial(true);
    player->Init();
    player->SetPosition(Vec3(-19, 35, 0));

    // ロード％更新
    AddLoadPercent(1.0f);

    // ステージ
    
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
    isTutorial = true;
    isPause = false;
    camTargetPos = { -19,5,0 };
    cameraTargetChange = false;    
    isSlow = false;    
    changeScene = false;

    radian = 0.0f;
    tutorialTick = 0;	// アニメーション用チック	
    stickAnim = 0;	// アニメーション
    isKeybord = false;

    renderSlow = false;
    renderAttack = false;
    renderMove = false;
    renderJump = false;
    renderSB = false;

    // 最初はプレイヤー操作不可 スロー入力して弾き返してから動ける   
    player->SetIsControl(false);
    player->SetCanSlow(false);
    player->SetCanAttack(false);
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

// 更新処理
void SceneTutorial::Update(float elapsedTime)
{
    BGM.get()->Play(0.5f);
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    // キーボード使用状態の取得
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

        // チュートリアル
        {
            // 着地したらスローできる
            if (player->Ground() && isTutorial)
            {
                isPause = true;                
                player->SetCanSlow(true);

                // 操作説明：スロー
                renderSlow = true;
            }            
            // スロー入力したらそのままスロー
            if (gamePad.GetButton() & GamePad::BTN_LEFT_TRIGGER && isTutorial && player->GetCanSlow())
            {
                isSlow = true;
                isPause = false;                
            }
            if (isSlow && isTutorial)slowElapsedTime = elapsedTime * 0.25f;

            // 弾止める
            for (int i = 0; i < EnemyBulletManager::Instance().GetProjectileCount(); i++)
            {
                float posX = EnemyBulletManager::Instance().GetProjectile(i)->GetPosition().x;
                if (posX <= -15.0f && isTutorial)
                {
                    EnemyBulletManager::Instance().GetProjectile(i)->SetIsMove(false);
                    isPause = true;
                    player->SetCanAttack(true);
                    
                    renderSlow = false;
                    // 操作説明：攻撃
                    renderAttack = true;                   
                }
            }

            // 穴に落ちたら
            if (player->GetPosition().x < -30.0f && renderMove)
            {
                renderMove = false;
                // 操作説明：ジャンプ
                renderJump = true;
            }
            else if (player->GetPosition().x < -60.0f && renderJump)
            {
                renderJump = false;
            }
            // SB位置についたら
            if (player->GetPosition().x < -85.0f && player->GetPosition().x > -140.0f && !renderSB)
            {                
                // 操作説明：SB
                renderSB = true;
            }
            else if((player->GetPosition().x > -85.0f || player->GetPosition().x < -140.0f ))
            {                                
                renderSB = false;
            }            
        }

        // ステージ
        StageManager::Instance().Update(slowElapsedTime);

        // プレイヤー
        {            
            player->Update(slowElapsedTime);
            player->SetSlowFixation(isSlow);
            player->SetIsTutorial(isTutorial);
            // シフトブレイク更新処理
            SBManager::Instance().Update(slowElapsedTime);

            
            // チュートリアル終わり
            if (player->GetIsAtk() && isTutorial) 
            {                
                isTutorial = false;
                isPause = false;
                isSlow = false;

                // カメラのターゲット変える
                cameraTargetChange = true;

                // 操作可能
                player->SetIsControl(true);
                player->SetSlowFixation(isSlow);

                // 弾動かす
                for (int i = 0; i < EnemyBulletManager::Instance().GetProjectileCount(); i++)
                {
                    EnemyBulletManager::Instance().GetProjectile(i)->SetIsMove(true);
                }

                renderAttack = false;
                // 操作説明：移動
                renderMove = true;
            }
        }

        // カメラ
        {
            CameraManager& cameraMgr = CameraManager::Instance();

            cameraMgr.Update(slowElapsedTime);

            Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
            if(cameraTargetChange) CameraManager::Instance().SetGoal(target);
            else CameraManager::Instance().SetGoal(camTargetPos);
        }


        // エネミー
        if (!player->GetClock())
        {
            EnemyManager::Instance().SetPlayer(player.get());           
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
    // フェードイン終わったら初期化
    if (Fade::Instance().GetFadeInFinish()) Fade::Instance().Initialize();

    // Menu
    Menu::Instance().Update(elapsedTime);
    // Fade
    Fade::Instance().Update(elapsedTime);
    // Goal
    Goal::Instance().SetPlayerPos(player->GetCenterPosition());
    Goal::Instance().Update(elapsedTime);


    // 現在のステージの死んでるエネミーの数が０の場合
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        // 全滅させてる場合操作説明ださない
        renderSB = false;
        Goal::Instance().SetCanGoal(true);

        // ゴールと判定とる         
        if (StageManager::Instance().CollisionPlayerVsNextStagePos(player->GetCenterPosition(), player->GetRadius()))
        {
            changeScene = true;
            // フェードアウト
            if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);            
        }
    }
    if(Menu::Instance().GetChangeFlag())
    {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
    }
    // フェードアウトおわったら次のシーンにいける
    if (changeScene && Fade::Instance().GetFadeOutFinish())
    {
        BGM.get()->Stop();
        // 次のステージへ移る処理
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

    framebuffer[0]->clear(dc, 1.0f, 1.0f, 1.0f, 0.0f);
    framebuffer[0]->activate(dc);
    {
        // モデル描画
        {
            // ステージ描画
            StageManager::Instance().Render(dc, elapsedTime);           

            // スロー演出、敵や自機など重要なオブジェクト以外を暗くする
            fade->render(dc, 0, 0, 1920, 1080, 1, 1, 1, player->GetSlowAlpha(), 0);

            // プレイヤー描画
            player->Render(dc);
            // エネミー描画
            EnemyManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());
            EnemyBulletManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());
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

        // 操作説明用
        RenderTutorial();

        // マウス位置
        Mouse& mouse = Input::Instance().GetMouse();
        Vec2 screenPosition = { static_cast<float>(mouse.GetPositionX()) ,static_cast<float>(mouse.GetPositionY()) };
        if (Input::Instance().GetGamePad().GetUseKeybord())cursorSprite->render(dc, screenPosition.x - 32, screenPosition.y - 32, 64, 64);
        // UI
        Bar->render(dc, 0, 0, 600, 300, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 208, 105, 344 * w, 78, 1.0f, 1.0f, 1.0f, 1.0f, 0);

        // Goal
        Goal::Instance().Render(dc);

        // メニュー
        Menu::Instance().Render(elapsedTime);

        // フェード用
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

// playerが死んだとき 等のリセット用
void SceneTutorial::Reset()
{
    // プレイヤー蘇生 ポジションリセット    
    player->Init();
    player->SetPosition(Vec3(-19, 35.0f, 0));    
  
    // ゴール不可
    Goal::Instance().SetCanGoal(false);

    // たまなし
    EnemyBulletManager::Instance().Clear();
    SBManager::Instance().Clear();
    // 敵蘇生 ポジションリセット
    EnemyManager::Instance().EnemyReset();  
    EnemyManager::Instance().Init();

    // 変数初期化
    isTutorial = true;
    isPause = false;
    camTargetPos = { -19,5,0 };
    cameraTargetChange = false;
    isSlow = false;

    radian = 0.0f;
    tutorialTick = 0;	// アニメーション用チック	
    stickAnim = 0;	// アニメーション    

    renderSlow = false;
    renderAttack = false;
    renderMove = false;
    renderJump = false;
    renderSB = false;


    // 最初はプレイヤー操作不可 スロー入力して弾き返してから動ける   
    player->SetIsControl(false);
    player->SetCanSlow(false);
    player->SetCanAttack(false);
    player->SetIsTutorial(isTutorial);
}

// 敵の初期化
void SceneTutorial::EnemyInitialize(ID3D11Device* device)
{
    for (int i = 0; i < ENEMY_MAX; i++)
    {
        if (ENEMY_MAX == i)
        {
            // ロード％更新
            AddLoadPercent(1.0f);
        }

        // 近接
        if (i == 1)
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
        else
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

// チュートリアル画像描画
void SceneTutorial::RenderTutorial()
{
    // チック
    float tutorialTick2 = 0.0f;
    int oldTick = tutorialTick;

    radian += 0.1f;
    if (radian > 3.1415f) radian = 0.0f;
    // アニメーション用１か０か
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
    // ビューポート
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);
    // 変換行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
    // プレイヤーの頭上に表示   
    DirectX::XMFLOAT3 worldPosition = player->GetCenterPosition();
    worldPosition.y += player->GetHeight();
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

    // 操作説明の画像描画

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
