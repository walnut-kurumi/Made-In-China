#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTutorial.h"
#include "SceneGameSt2.h"
#include "Graphics/Graphics.h"


// 初期化
void SceneTitle::Initialize()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();

    SEDecision = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Decision.wav", false);
    BGM = Audio::Instance().LoadAudioSource("Data\\Audio\\BGM\\title.wav", true);

    titleSprite = new Sprite(device, L"./Data/Sprites/title.png");
    cursorSprite = new Sprite(device, L"./Data/Sprites/cursor.png");
    gameStart = new Sprite(device, L"./Data/Sprites/scene//start.png");
    gameEnd = new Sprite(device, L"./Data/Sprites/scene//end.png");


    start = true;
    end = false;
    startAlpha = 1.0f;
    endAlpha = 0.4f;
    startsize = { 320,180 };
    endsize = { 320,180 };    
    startpos = { 160,505 };
    endpos = { 613,505 };


    Input::Instance().GetMouse().SetMoveCursor(true);

    Fade::Instance().Initialize();
    //BGM = Audio::Instance().LoadAudioSource("Data\\Audio\\BGM\\title.wav", true); 
}

// 終了化
void SceneTitle::Finalize()
{
    Fade::Instance().Finalize();

    delete titleSprite;
    delete cursorSprite;
    delete gameStart;
    delete gameEnd;
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
    BGM.get()->Play(1.0f);

    GamePad& gamePad = Input::Instance().GetGamePad();    
    // なにかボタンを押したらゲームシーン切り替え
    const GamePadButton anyButton =
        GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_BACK        
        | GamePad::BTN_LEFT_SHOULDER
        | GamePad::BTN_LEFT_THUMB
        | GamePad::BTN_LEFT_TRIGGER        
        | GamePad::BTN_RIGHT_SHOULDER
        | GamePad::BTN_RIGHT_THUMB
        | GamePad::BTN_RIGHT_TRIGGER
        | GamePad::BTN_START        
        | GamePad::BTN_X
        | GamePad::BTN_Y;

    Mouse& mouse = Input::Instance().GetMouse();    

    //マウス左クリックでマップ選択
    const mouseButton mouseClick =
        Mouse::BTN_LEFT;

    if (mouse.GetButtonDown() & mouseClick) gamePad.SetUseKeybord(true);

    // scene選択
    SceneSelect();
   
    
    Fade::Instance().Update(elapsedTime);

    // すすむ
    {
        if (start && (gamePad.GetButtonUp() & anyButton || mouse.GetButtonUp() & mouseClick))
        {
           SEDecision->Play(0.5f);
           BGM.get()->Stop();

            // フェードアウトする
            Fade::Instance().SetFadeOutFlag(true);
        }
        // フェードアウト終わったら
        if (Fade::Instance().GetFadeOutFinish())
        {
           SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTutorial));
           //SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGameSt2));
        }
    }
    // おわる
    {
        if (end && (gamePad.GetButtonUp() & anyButton || mouse.GetButtonUp() & mouseClick))
        {
            SEDecision.get()->Play(0.5f);
            BGM.get()->Stop();            

            DestroyWindow(GetActiveWindow());
        }
    }

}

// 描画処理
void SceneTitle::Render(float elapsedTime)
{
    ID3D11Device* device = Graphics::Ins().GetDevice();
    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
    Graphics& gfx = Graphics::Ins();

    ID3D11RenderTargetView* rtv = gfx.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = gfx.GetDepthStencilView();


    FLOAT color[] = { 0.6f,0.6f,0.6f,1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // 通常レンダリング
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 2D描画
    {
        titleSprite->render(dc, 0, 0, gfx.GetScreenWidth(), gfx.GetScreenHeight());        
        gameStart->render(dc, startpos.x, startpos.y, startsize.x, startsize.y, 1, 1, 1, startAlpha, 0);
        gameEnd->render(dc, endpos.x, endpos.y, endsize.x, endsize.y, 1, 1, 1, endAlpha, 0);
        //cursorSprite->render(dc, mousepos.x-12, mousepos.y-12, 23, 24, 1, 1, 1, 1, 0);

        Fade::Instance().Render(elapsedTime);
    }

    // モデル描画
    {
    }

    // デバック
    {
    }
}

void SceneTitle::SceneSelect()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    DirectX::XMFLOAT3 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetPositionY());
    mousepos.x = screenPosition.x;
    mousepos.y = screenPosition.y;

    const GamePadButton Left =
        GamePad::BTN_LEFT;
    const GamePadButton Right =
        GamePad::BTN_RIGHT;

    if (screenPosition.y >= startpos.y && screenPosition.y < startpos.y + startsize.y)
    {
        if (screenPosition.x >= startpos.x && screenPosition.x <= startpos.x + startsize.x)
        {
            start = true;
            end = false;
        }
        if (screenPosition.x >= endpos.x && screenPosition.x <= endpos.x + endsize.x)
        {
            start = false;
            end = true;
        }
    }
    if (gamePad.GetButtonDown() & Left || gamePad.GetButtonDown() & Right)
    {
        start = !start;
        end = !end;
    }

    if (gamePad.GetAxisLX() > 0.0f )
    {
        start = false;
        end = true;
    }
    if (gamePad.GetAxisLX() < 0.0f)
    {
        start = true;
        end = false;
    }

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
