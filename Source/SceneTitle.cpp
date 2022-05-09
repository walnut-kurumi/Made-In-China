#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTutorial.h"
#include "SceneGame.h"
#include "Graphics/Graphics.h"


// ������
void SceneTitle::Initialize()
{
    ID3D11Device* device = Graphics::Ins().GetDevice();
    titleSprite = new Sprite(device, L"./Data/Sprites//title.png");
    cursorSprite = new Sprite(device, L"./Data/Sprites//cursor.png");
    gameStart = new Sprite(device, L"./Data/Sprites/scene//start.png");
    gameEnd = new Sprite(device, L"./Data/Sprites/scene//end.png");

    start = true;
    end = false;

    Input::Instance().GetMouse().SetMoveCursor(true);

    Fade::Instance().Initialize();
}

// �I����
void SceneTitle::Finalize()
{
    Fade::Instance().Finalize();

    delete titleSprite;
    delete cursorSprite;
    delete gameStart;
    delete gameEnd;
}

// �X�V����
void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();    
    // �Ȃɂ��{�^������������Q�[���V�[���؂�ւ�
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

    //�}�E�X���N���b�N�Ń}�b�v�I��
    const mouseButton mouseClick =
        Mouse::BTN_LEFT;

    // �����
    {
        if (end && (gamePad.GetButtonDown() & anyButton || mouse.GetButtonDown() & mouseClick))
        {
            DestroyWindow(GetActiveWindow());
        }
    }

    const GamePadButton updown =
        GamePad::BTN_UP
        | GamePad::BTN_DOWN
        | GamePad::BTN_W
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

    SceneSelect();
    
    Fade::Instance().Update(elapsedTime);

    // ������
    {
        if (start && (gamePad.GetButtonDown() & anyButton || mouse.GetButtonDown() & mouseClick))
        {
            // �t�F�[�h�A�E�g����
            Fade::Instance().SetFadeOutFlag(true);
        }
        // �t�F�[�h�A�E�g�I�������
        if (Fade::Instance().GetFadeOutFinish())
        {
            //SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTutorial));
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
        }
    }


}

// �`�揈��
void SceneTitle::Render(float elapsedTime)
{
    ID3D11Device* device = Graphics::Ins().GetDevice();
    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
    Graphics& gfx = Graphics::Ins();

    // 2D�`��
    {
        titleSprite->render(dc, 0, 0, gfx.GetScreenWidth(), gfx.GetScreenHeight());        
        gameStart->render(dc, startpos.x, startpos.y, startsize.x, startsize.y, 1, 1, 1, startAlpha, 0);
        gameEnd->render(dc, endpos.x, endpos.y, endsize.x, endsize.y, 1, 1, 1, endAlpha, 0);
        cursorSprite->render(dc, mousepos.x-12, mousepos.y-12, 23, 24, 1, 1, 1, 1, 0);

        Fade::Instance().Render(elapsedTime);
    }

    // ���f���`��
    {
    }

    // �f�o�b�N
    {
    }
}

void SceneTitle::SceneSelect()
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
