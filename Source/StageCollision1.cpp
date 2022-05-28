#include "StageCollision1.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageCollision1::StageCollision1(ID3D11Device* device)
{
   
    model = new Model(device, ".\\Data\\Models\\Stage\\Stag1_collision.fbx", true, 0);
    
    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;
    scale.y *= 0.7f;

    angle.y = DirectX::XMConvertToRadians(-90);

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageCollision1::~StageCollision1()
{
    delete model;
}

void StageCollision1::Update(float elapsedTime)
{    
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageCollision1::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{

}

bool StageCollision1::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}