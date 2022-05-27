#include "SceneClear.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTutorial.h"
#include "Graphics/Graphics.h"
#include "SceneTitle.h"


// ������
void SceneClear::Initialize()
{
    SEDecision = Audio::Instance().LoadAudioSource("Data\\Audio\\SE\\Decision.wav", false);
    BGM = Audio::Instance().LoadAudioSource("Data\\Audio\\BGM\\title.wav", true);
    ID3D11Device* device = Graphics::Ins().GetDevice();
    clearSprite = new Sprite(device, L"./Data/Sprites/clear.png");
    cursorSprite = new Sprite(device, L"./Data/Sprites/cursor.png");
    gameStart = new Sprite(device, L"./Data/Sprites/scene/retry.png");
    gameEnd = new Sprite(device, L"./Data/Sprites/scene/end.png");
  
    start = true;
    end = false;
    startAlpha = 1.0f;
    endAlpha = 0.4f;
    startsize = { 320,180 };
    endsize = { 320,180 };
    startpos = { Graphics::Ins().GetScreenWidth() / 2 - startsize.x / 2 - 300,Graphics::Ins().GetScreenHeight() / 2 - startsize.y / 2 +180};
    endpos = { startpos.x + 500,startpos.y };

    Input::Instance().GetMouse().SetMoveCursor(true);

    Fade::Instance().Initialize();
}

// �I����
void SceneClear::Finalize()
{
    delete clearSprite;
    delete cursorSprite;
    delete gameStart;
    delete gameEnd;
}

// �X�V����
void SceneClear::Update(float elapsedTime)
{
    BGM.get()->Play(0.6f);
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

    // scene�I��
    SceneSelect();

    Fade::Instance().Update(elapsedTime);

    // ������
    {
        if (start && (gamePad.GetButtonUp() & anyButton || mouse.GetButtonUp() & mouseClick))
        {
            SEDecision.get()->Play(0.5f);
            // �t�F�[�h�A�E�g����
            Fade::Instance().SetFadeOutFlag(true);
        }
        // �t�F�[�h�A�E�g�I�������
        if (Fade::Instance().GetFadeOutFinish())
        {
            BGM.get()->Stop();
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
        }
    }
    // �����
    {
        if (end && (gamePad.GetButtonUp() & anyButton || mouse.GetButtonUp() & mouseClick))
        {
            SEDecision.get()->Play(0.5f);
            BGM.get()->Stop();
            DestroyWindow(GetActiveWindow());
        }
    }

}

// �`�揈��
void SceneClear::Render(float elapsedTime)
{
    ID3D11Device* device = Graphics::Ins().GetDevice();
    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
    Graphics& gfx = Graphics::Ins();

    // 2D�`��
    {
        clearSprite->render(dc, 0, 0, gfx.GetScreenWidth(), gfx.GetScreenHeight());        
        gameStart->render(dc, startpos.x, startpos.y, startsize.x, startsize.y, 1, 1, 1, startAlpha, 0);
        gameEnd->render(dc, endpos.x, endpos.y, endsize.x, endsize.y, 1, 1, 1, endAlpha, 0);
        cursorSprite->render(dc, mousepos.x - 12, mousepos.y - 12, 23, 24, 1, 1, 1, 1, 0);
    }

    // ���f���`��
    {
    }

    // �f�o�b�N
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

    if (gamePad.GetAxisLX() > 0.0f)
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
