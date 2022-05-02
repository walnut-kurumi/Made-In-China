#include "Fade.h"
#include "Input/Input.h"

// �R���X�g���N�^
Fade::Fade()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();

    fade = std::make_unique<Sprite>(device, L"./Data/Sprites/scene/black.png");
}

// ������
void Fade::Initialize()
{    
   fadeInFlag = false;
   fadeOutFlag = false;
   fadeInFinish = false;
   fadeOutFinish = false;
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
 
    if (fadeInFlag)   FadeIn(0.02f);
    if (fadeOutFlag) FadeOut(0.02f);

}

// �`�揈��
void Fade::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();
    // 2D�`��
    {                
        {
            fade.get()->render(dc, 0, 0, 1920, 1080, 1, 1, 1, fadeAlpha, 0);
        }
    }
}

// �t�F�[�h�C��
void Fade::FadeIn(float speed)
{  
    fadeInFinish = false;    

    // ���邭���Ă��i�����ɂ��Ă��j
    fadeAlpha -= speed;

    if (fadeAlpha <= 0.0f)
    {
        fadeInFinish = true;
        fadeInFlag = false;
        
        fadeOutFlag = false;
    }
}

// �t�F�[�h�A�E�g
void Fade::FadeOut(float speed)
{
    fadeOutFinish = false;    

    // �Â����Ă����i�A���t�@�l������j
    fadeAlpha += speed;

    if (fadeAlpha >= 1.5f)
    {
        fadeOutFinish = true;
        fadeOutFlag = false;

        fadeInFinish = false;                
    }
}


