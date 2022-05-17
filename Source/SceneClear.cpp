#include "SceneClear.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTutorial.h"
#include "Graphics/Graphics.h"


// 初期化
void SceneClear::Initialize()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();
    clearSprite = new Sprite(device, L"./Data/Sprites//clear.png");
    cursorSprite = new Sprite(device, L"./Data/Sprites//cursor.png");
    gameStart = new Sprite(device, L"./Data/Sprites/scene//retry.png");
    gameEnd = new Sprite(device, L"./Data/Sprites/scene//end.png");
  
    start = true;
    end = false;
    startAlpha = 1.0f;
    endAlpha = 0.4f;
    startsize = { 320,180 };
    endsize = { 320,180 };
    startpos = { Graphics::Ins().GetScreenWidth() / 2 - startsize.x / 2,Graphics::Ins().GetScreenHeight() / 2 - startsize.y / 2 + 60 };
    endpos = { startpos.x,startpos.y + endsize.y * 1.2f };

    Input::Instance().GetMouse().SetMoveCursor(true);

    Fade::Instance().Initialize();
}

// 終了化
void SceneClear::Finalize()
{
    delete clearSprite;
    delete cursorSprite;
    delete gameStart;
    delete gameEnd;
}

// 更新処理
void SceneClear::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    // なにかボタンを押したらゲームシーン切り替え
    const GamePadButton anyButton =
        GamePad::BTN_A
        | GamePad::BTN_B
        | GamePad::BTN_BACK
        //| GamePad::BTN_DOWN
        | GamePad::BTN_LEFT
        | GamePad::BTN_LEFT_SHOULDER
        | GamePad::BTN_LEFT_THUMB
        | GamePad::BTN_LEFT_TRIGGER
        | GamePad::BTN_RIGHT
        | GamePad::BTN_RIGHT_SHOULDER
        | GamePad::BTN_RIGHT_THUMB
        | GamePad::BTN_RIGHT_TRIGGER
        | GamePad::BTN_START
        //| GamePad::BTN_UP
        | GamePad::BTN_X
        | GamePad::BTN_Y;

    Mouse& mouse = Input::Instance().GetMouse();
    DirectX::XMFLOAT3 screenPosition;
    screenPosition.x = static_cast<float>(mouse.GetPositionX());
    screenPosition.y = static_cast<float>(mouse.GetPositionY());
    mousepos.x = screenPosition.x;
    mousepos.y = screenPosition.y;

    //マウス左クリックでマップ選択
    const mouseButton mouseClick =
        Mouse::BTN_LEFT;

    // scene選択
    SceneSelect();

    Fade::Instance().Update(elapsedTime);

    // すすむ
    {
        if (start && (gamePad.GetButtonUp() & anyButton || mouse.GetButtonUp() & mouseClick))
        {
            // フェードアウトする
            Fade::Instance().SetFadeOutFlag(true);
        }
        // フェードアウト終わったら
        if (Fade::Instance().GetFadeOutFinish())
        {
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTutorial));
        }
    }
    // おわる
    {
        if (end && (gamePad.GetButtonUp() & anyButton || mouse.GetButtonUp() & mouseClick))
        {
            DestroyWindow(GetActiveWindow());
        }
    }

}

// 描画処理
void SceneClear::Render(float elapsedTime)
{
    ID3D11Device* device = Graphics::Ins().GetDevice();
    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
    Graphics& gfx = Graphics::Ins();

    // 2D描画
    {
        clearSprite->render(dc, 0, 0, gfx.GetScreenWidth(), gfx.GetScreenHeight());        
        gameStart->render(dc, startpos.x, startpos.y, startsize.x, startsize.y, 1, 1, 1, startAlpha, 0);
        gameEnd->render(dc, endpos.x, endpos.y, endsize.x, endsize.y, 1, 1, 1, endAlpha, 0);
        //cursorSprite->render(dc, mousepos.x - 12, mousepos.y - 12, 23, 24, 1, 1, 1, 1, 0);
    }

    // モデル描画
    {
    }

    // デバック
    {
    }
}

void SceneClear::SceneSelect()
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

    if (screenPosition.x >= startpos.x && screenPosition.x < startpos.x + startsize.x)
    {
        if (screenPosition.y >= startpos.y && screenPosition.y <= startpos.y + startsize.y)
        {
            start = true;
            end = false;
        }
        if (screenPosition.y >= endpos.y && screenPosition.y <= endpos.y + endsize.y)
        {
            start = false;
            end = true;
        }
    }
    if (gamePad.GetButtonDown() & up || gamePad.GetButtonDown() & down)
    {
        start = !start;
        end = !end;
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
