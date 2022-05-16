#include "StageMain0.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageMain0::StageMain0(ID3D11Device* device)
{
    back = new Sprite(device, L"./Data/Sprites/BackGround.png");

    model = new Model(device, ".\\Data\\Models\\Stage\\Stage0.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;

    angle.y = DirectX::XMConvertToRadians(-90);

    bgpos = { -200, -600 };

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageMain0::~StageMain0()
{
    delete model;
    delete back;
}

void StageMain0::Update(float elapsedTime)
{
    bgpos = { player->GetPosition().x - 20,player->GetPosition().y };
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageMain0::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    //Scroll.data.scroll_direction;
    //deviceContext->UpdateSubresource(
    back->render(deviceContext, bgpos.x, -150, 1500, 1150, 1.0f, 1.0f, 1.0f, 1.0f, 0);

    model->Begin(deviceContext, Shaders::Ins()->GetSkinnedMeshShader());
    model->Render(deviceContext);

    // 必要なったら追加
    debugRenderer.get()->DrawSphere(position, 25, Vec4(1, 0, 1, 1));

    //debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageMain0::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;
    //return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}