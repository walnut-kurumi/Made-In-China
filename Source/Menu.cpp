#include "Menu.h"
#include "Input/Input.h"


// 初期化
void Menu::Initialize()
{    
    ID3D11Device* device = Graphics::Ins().GetDevice();

    gameStart = new Sprite(device, L"./Data/Sprites/scene//start.png");
    gameEnd = new Sprite(device, L"./Data/Sprites/scene//end.png");    

    start = true;
    end = false;   
}

// 終了化
void Menu::Finalize()
{   
    delete gameEnd;
    delete gameStart;
}

// 更新処理
void Menu::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

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
            gameStart->render(dc, startpos.x, startpos.y, startsize.x, startsize.y, 1, 1, 1, startAlpha, 0);
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


void Menu::SceneSelect()
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
