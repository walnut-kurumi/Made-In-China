#include "StageMain1.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageMain1::StageMain1(ID3D11Device* device)
{
    back = new Sprite(device, L"./Data/Sprites/BackGround.png");

    model = new Model(device, ".\\Data\\Models\\Stage\\Stage1.fbx", true, 0);
   
    scale.x = scale.y = scale.z = 0.05f;
    scale.x *= -1;
    scale.y *= 0.7f;

    angle.y = DirectX::XMConvertToRadians(-90);

    bgpos = { -200, -600 };

    NextStagePos = { -168.0f,35.5f,0.0f };

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageMain1::~StageMain1()
{
    delete model;
    delete back;
}

void StageMain1::Update(float elapsedTime)
{    
    bgpos = { player->GetPosition().x - 50,player->GetPosition().y };
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageMain1::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    back->render(deviceContext, bgpos.x, -430, 1500, 1150, 1.0f, 1.0f, 1.0f, 1.0f, 0);    

    model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    model->Render(deviceContext);

#ifdef _DEBUG
    debugRenderer.get()->DrawSphere(NextStagePos, NextStageRadius, Vec4(0, 0.5f, 1, 1));
    debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
#endif
}

bool StageMain1::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;    
}