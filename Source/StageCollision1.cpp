#include "StageCollision1.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageCollision1::StageCollision1(ID3D11Device* device)
{
   
    //model = new Model(device, ".\\Data\\Models\\Stage\\Stage1_collision.fbx", true, 0);
    model = new Model(device, ".\\Data\\Models\\Stage\\newStages\\Stage1_collision.fbx", true, 0);
    //model = new Model(device, ".\\Data\\Models\\Stage\\oldStages\\MDL_stage_atari.fbx", true, 0);

    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;

    angle.y = DirectX::XMConvertToRadians(-90);

    type = Type::Penetrate;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageCollision1::~StageCollision1()
{
    delete model;
}

void StageCollision1::Update(float elapsedTime)
{
    scale.x = scale.y = scale.z = 0.05f;
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageCollision1::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    //Scroll.data.scroll_direction;
    //deviceContext->UpdateSubresource(
    //model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    //model->Render(deviceContext);

    // •K—v‚È‚Á‚½‚ç’Ç‰Á
    //debugRenderer.get()->DrawSphere(position, 25, Vec4(1, 0, 1, 1));

    //debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageCollision1::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    //return false;
    return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}