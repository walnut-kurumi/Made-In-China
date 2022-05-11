#include "StageMain2.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageMain2::StageMain2(ID3D11Device* device)
{
    back = new Sprite(device, L"./Data/Sprites/BackGround.png");

    model = new Model(device, ".\\Data\\Models\\Stage\\Stage2.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;

    position.x = -140;
    position.y = -20;
    position.z = 5;

    angle.y = DirectX::XMConvertToRadians(-90);

    bgpos = { -200, -600 };

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageMain2::~StageMain2()
{
    delete model;
    delete back;
}

void StageMain2::Update(float elapsedTime)
{
    bgpos = { player->GetPosition().x - 20,player->GetPosition().y };
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageMain2::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    //Scroll.data.scroll_direction;
    //deviceContext->UpdateSubresource(
    back->render(deviceContext, bgpos.x, -150, 1500, 1150, 1.0f, 1.0f, 1.0f, 1.0f, 0);

    model->Begin(deviceContext, Shaders::Ins()->GetSkinnedMeshShader());
    model->Render(deviceContext);

    // •K—v‚È‚Á‚½‚ç’Ç‰Á
    debugRenderer.get()->DrawSphere(position, 25, Vec4(1, 0, 1, 1));

    //debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageMain2::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;
    //return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}