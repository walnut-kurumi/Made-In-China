#include "Fade.h"
#include "Input/Input.h"


// ������
void Fade::Initialize()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();

    fade = std::make_unique<Sprite>(device, L"./Data/Sprites/scene/black.png");

   fadeInFlag = false;
   fadeOutFlag = false;
   fadeAlpha = 0.0f;
}

// �I����
void Fade::Finalize()
{   
}

// �X�V����
void Fade::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    
}

// �`�揈��
void Fade::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();
    // 2D�`��
    {        
        if (fadeInFlag || fadeOutFlag)
        {
            fade.get()->render(dc, 0, 0, 1920, 1080, 1, 1, 1, fadeAlpha, 0);
        }
    }
}

// �t�F�[�h�C��
void Fade::FadeIn(float speed)
{  
    if (fadeAlpha >= 1.0f)fadeAlpha -= speed;
}

// �t�F�[�h�A�E�g
void Fade::FadeOut(float speed)
{
}

// ���Z�b�g
void Fade::ResetFade()
{
    fadeInFlag = false;
    fadeOutFlag = false;
    fadeAlpha = 0.0f;
}

