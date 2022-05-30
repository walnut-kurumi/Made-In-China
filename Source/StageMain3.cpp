#include "StageMain3.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageMain3::StageMain3(ID3D11Device* device)
{
    back = new Sprite(device, L"./Data/Sprites/BackGround.png");

    model = new Model(device, ".\\Data\\Models\\Stage\\Stage3\\Stage3_3.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;
    //scale.y *= 0.7f;
    angle.y = DirectX::XMConvertToRadians(-90);

    bgpos = { -200, -600 };

    NextStagePos = { -176.0f,50.05f,0.0f };

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageMain3::~StageMain3()
{
    delete model;
    delete back;
}

void StageMain3::Update(float elapsedTime)
{
    bgpos = { player->GetPosition().x - 40,player->GetPosition().y };
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageMain3::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{    
    back->render(deviceContext, bgpos.x, -400, 1500, 1150, 1.0f, 1.0f, 1.0f, 1.0f, 0);

    model->Begin(deviceContext, Shaders::Ins()->GetSkinnedMeshShader());
    model->Render(deviceContext);

#ifdef _DEBUG
    debugRenderer.get()->DrawSphere(NextStagePos, NextStageRadius, Vec4(0, 0.5f, 1, 1));
    debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
#endif
}

bool StageMain3::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;
}