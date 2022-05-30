#include "StageMain4.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

#include "Framework.h"

StageMain4::StageMain4(ID3D11Device* device)
{
    back = new Sprite(device, L"./Data/Sprites/BackGround.png");

    model = new Model(device, ".\\Data\\Models\\Stage\\Stage3\\Stage3_3.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;
    //scale.y *= 0.7f;
    angle.y = DirectX::XMConvertToRadians(-90);

    bgpos = { -200, -600 };

    NextStagePos = { -164.0f,60.05f,0.0f };

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageMain4::~StageMain4()
{
    delete model;
    delete back;
}

void StageMain4::Update(float elapsedTime)
{
    bgpos = { player->GetPosition().x - 40,player->GetPosition().y };
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageMain4::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    back->render(deviceContext, bgpos.x, -400, 1500, 1150, 1.0f, 1.0f, 1.0f, 1.0f, 0);

    model->Begin(deviceContext, Shaders::Ins()->GetSkinnedMeshShader());
    model->Render(deviceContext);



#ifdef _DEBUG
    Vec3 collision = { -125.0f,34.0f,0.0f };

    debugRenderer.get()->DrawSphere(NextStagePos, NextStageRadius, Vec4(0, 0.5f, 1, 1));
    debugRenderer.get()->DrawSphere(collision, 3.0f, Vec4(0, 0.5f, 1, 1));
    debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
#endif
}

bool StageMain4::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;
}