#include "StageCollision0.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageCollision0::StageCollision0(ID3D11Device* device)
{

    model = new Model(device, ".\\Data\\Models\\Stage\\MDL_stage_atari.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;

    position.x = -140;
    position.y = -20;
    position.z = 5;

    angle.y = DirectX::XMConvertToRadians(-90);

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageCollision0::~StageCollision0()
{
    delete model;
}

void StageCollision0::Update(float elapsedTime)
{
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageCollision0::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    //Scroll.data.scroll_direction;
    //deviceContext->UpdateSubresource(
    //model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    //model->Render(deviceContext);

    // •K—v‚È‚Á‚½‚ç’Ç‰Á
    //debugRenderer.get()->DrawSphere(position, 25, Vec4(1, 0, 1, 1));

    //debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageCollision0::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    //return false;
    return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}