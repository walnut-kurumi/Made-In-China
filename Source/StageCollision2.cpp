#include "StageCollision2.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageCollision2::StageCollision2(ID3D11Device* device)
{

    model = new Model(device, ".\\Data\\Models\\Stage\\Stage2_collision.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;

    angle.y = DirectX::XMConvertToRadians(-90);
    
    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageCollision2::~StageCollision2()
{
    delete model;
}

void StageCollision2::Update(float elapsedTime)
{
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageCollision2::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    //Scroll.data.scroll_direction;
    //deviceContext->UpdateSubresource(
    //model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    //model->Render(deviceContext);

    // •K—v‚È‚Á‚½‚ç’Ç‰Á
    //debugRenderer.get()->DrawSphere(position, 25, Vec4(1, 0, 1, 1));

    //debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageCollision2::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    //return false;
    return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}