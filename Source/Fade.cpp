#include "Fade.h"
#include "Input/Input.h"

// コンストラクタ
Fade::Fade()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();

    fade = std::make_unique<Sprite>(device, L"./Data/Sprites/scene/black.png");
}

// 初期化
void Fade::Initialize()
{    
   fadeInFlag = false;
   fadeOutFlag = false;
   fadeInFinish = false;
   fadeOutFinish = false;
   fadeAlpha = 0.0f;
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
 
    if (fadeInFlag)   FadeIn(0.02f);
    if (fadeOutFlag) FadeOut(0.02f);

}

// 描画処理
void Fade::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();
    // 2D描画
    {                
        {
            fade.get()->render(dc, 0, 0, 1920, 1080, 1, 1, 1, fadeAlpha, 0);
        }
    }
}

// フェードイン
void Fade::FadeIn(float speed)
{  
    fadeInFinish = false;    

    // 明るくしてく（透明にしてく）
    fadeAlpha -= speed;

    if (fadeAlpha <= 0.0f)
    {
        fadeInFinish = true;
        fadeInFlag = false;
        
        fadeOutFlag = false;
    }
}

// フェードアウト
void Fade::FadeOut(float speed)
{
    fadeOutFinish = false;    

    // 暗くしていく（アルファ値あげる）
    fadeAlpha += speed;

    if (fadeAlpha >= 1.5f)
    {
        fadeOutFinish = true;
        fadeOutFlag = false;

        fadeInFinish = false;                
    }
}


