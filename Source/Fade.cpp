#include "Fade.h"
#include "Input/Input.h"


// 初期化
void Fade::Initialize()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();

    fade = std::make_unique<Sprite>(device, L"./Data/Sprites/scene/black.png");
}

// 終了化
void Fade::Finalize()
{   
}

// 更新処理
void Fade::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    
}

// 描画処理
void Fade::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();
    // 2D描画
    {        
    }
}

// フェードイン
void Fade::FadeIn()
{  
}

// フェードアウト
void Fade::FadeOut()
{
}

