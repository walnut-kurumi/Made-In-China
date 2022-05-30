#include "StagePenetrate4.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StagePenetrate4::StagePenetrate4(ID3D11Device* device)
{

    model = new Model(device, ".\\Data\\Models\\Stage\\Stage3\\Stage3_penetrate.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;
    scale.y *= 0.7f;
    angle.y = DirectX::XMConvertToRadians(-90);

    type = Type::Penetrate;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StagePenetrate4::~StagePenetrate4()
{
    delete model;
}

void StagePenetrate4::Update(float elapsedTime)
{
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StagePenetrate4::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    model->Render(deviceContext);
}

bool StagePenetrate4::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}