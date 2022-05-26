#include "Goal.h"
#include "Graphics/Graphics.h"
#include "Camera/CameraManager.h"

Goal::~Goal()
{
    delete goalArrow;
}

void Goal::Init(ID3D11Device* device)
{
	goalArrow = new Sprite(device, L"./Data/Sprites/UI/Arrow.png");

	arrowAngle = 0;
    canGoal = false;
    movePosY = 0.0f;
    radian = 0.0f;
}

void Goal::Update(float elapsedTime)
{
    //ゴールの向き    
    Vec3 goalDirection = VecMath::Normalize(goalPos - playerPos);
    Vec3 X = VecMath::Normalize({ -1,0,0 });
    arrowAngle = DirectX::XMConvertToDegrees(acosf(VecMath::Dot(X,goalDirection)));
    if (playerPos.y < goalPos.y) arrowAngle *= -1;

    radian += 0.15f;    
    if (radian > 3.1415f) radian = 0.0f;    
    if(canGoal)
    {
        movePosY = sinf(radian) * 3.5f;
    }    
}

void Goal::Render(ID3D11DeviceContext* dc)
{
#ifdef USE_IMGUI   
    if (ImGui::Begin("Goal", nullptr, ImGuiWindowFlags_None)) {


        ImGui::InputFloat3("goal", &goalPos.x);
        ImGui::InputFloat3("player", &playerPos.x);
        ImGui::SliderFloat("Angle", &arrowAngle, 0, 3.14f);
        ImGui::Checkbox("canGoal", &canGoal);

    }
    ImGui::End();
#endif
    if (canGoal)
    {
        ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();
        // ビューポート
        D3D11_VIEWPORT viewport;
        UINT numViewports = 1;
        dc->RSGetViewports(&numViewports, &viewport);
        // 変換行列
        DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
        DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
        DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
        // プレイヤーの頭上に表示   
        DirectX::XMFLOAT3 worldPosition = playerPos;
        worldPosition.y += 8.0f;
        DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
        // ワールド座標からスクリーン座標へ変換
        DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
            WorldPosition,
            viewport.TopLeftX,
            viewport.TopLeftY,
            viewport.Width,
            viewport.Height,
            viewport.MinDepth,
            viewport.MaxDepth,
            Projection,
            View,
            World
        );
        // スクリーン座標
        DirectX::XMFLOAT2 screenPosition;
        DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

        goalArrow->render(dc, screenPosition.x - 40, screenPosition.y - 40 - movePosY, 80, 80, 1, 1, 1, 1, arrowAngle, 0, 0, 320, 320);
    }
}
