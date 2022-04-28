#include "Fade.h"
#include "Input/Input.h"


// ������
void Fade::Initialize()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();

    fade = std::make_unique<Sprite>(device, L"./Data/Sprites/scene/black.png");
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
    }
}

// �t�F�[�h�C��
void Fade::FadeIn()
{  
}

// �t�F�[�h�A�E�g
void Fade::FadeOut()
{
}

