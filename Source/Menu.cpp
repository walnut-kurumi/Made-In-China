#include "Menu.h"
#include "Input/Input.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneGame.h"

// 初期化
void Menu::Initialize()
{    
    //SEDecision = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Decision.wav", false);
    ID3D11Device* device = Graphics::Ins().GetDevice();

    gameRetry = new Sprite(device, L"./Data/Sprites/scene/retry.png");
    gameEnd = new Sprite(device, L"./Data/Sprites/scene/end.png");    
    gameMenu = new Sprite(device, L"./Data/Sprites/scene/menu.png");

    retry = true;
    end = false;   
    change = false;
    menuflag = false;
}

// 終了化
void Menu::Finalize()
{   
    delete gameEnd;
    delete gameRetry;
    delete gameMenu;
}

// 更新処理
void Menu::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    //ポーズメニュー
    if (gamePad.GetButtonDown() & GamePad::BTN_START)
    {
        if (!menuflag)
        {
            SEMenu = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Menu.wav", false);
            SEMenu.get()->Play(0.5f);
            menuflag = true;
        }
        else
        {
            SECancellation = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Cancellation.wav", false);
            SECancellation.get()->Play(0.5f);
            menuflag = false;
        }
    }
    if (menuflag == true)
    {
        menu();
        SceneSelect();
    }
}

// 描画処理
void Menu::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();  
    // 2D描画
    {       
        if (menuflag == true)
        {
            gameMenu->render(dc, 0, 0, gfx.GetScreenWidth(), gfx.GetScreenHeight());
            gameRetry->render(dc, retrypos.x, retrypos.y, retrysize.x, retrysize.y, 1, 1, 1, retryAlpha, 0);
            gameEnd->render(dc, endpos.x, endpos.y, endsize.x, endsize.y, 1, 1, 1, endAlpha, 0);
        }        
    }
}

//メニュー
void Menu::menu()
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


    if (retry && (gamePad.GetButtonUp() & GamePad::BTN_A || mouse.GetButtonUp() & mouseClick))
    {
        SEDecision = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Decision.wav", false);
        SEDecision.get()->Play(0.5f);
        //SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
        change = true;
    }
    else if (end && (gamePad.GetButtonUp() & GamePad::BTN_A || mouse.GetButtonDown() & mouseClick))
    {
        SEDecision = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Decision.wav", false);
        SEDecision.get()->Play(0.5f);
        DestroyWindow(GetActiveWindow());
    }
 
}


void Menu::SceneSelect()
{

    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    DirectX::XMFLOAT3 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetPositionY());
    mousepos.x = screenPosition.x;
    mousepos.y = screenPosition.y;

    const GamePadButton up =
        GamePad::BTN_UP
        | GamePad::BTN_W;
    const GamePadButton down =
        GamePad::BTN_DOWN
        | GamePad::BTN_S;

    if (screenPosition.x >= retrypos.x && screenPosition.x < retrypos.x + retrysize.x)
    {
        if (screenPosition.y >= retrypos.y && screenPosition.y <= retrypos.y + retrysize.y)
        {
            retry = true;
            end = false;
        }
        if (screenPosition.y >= endpos.y && screenPosition.y <= endpos.y + endsize.y)
        {
            retry = false;
            end = true;
        }
    }
    if (gamePad.GetButtonDown() & up || gamePad.GetButtonDown() & down)
    {
        retry = !retry;
        end = !end;
    }

    if (retry)
    {
        retryAlpha = 1.0f;
        endAlpha = 0.4f;
    }
    if (end)
    {
        retryAlpha = 0.4f;
        endAlpha = 1.0f;
    }
}
