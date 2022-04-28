#include "StageMain.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageMain::StageMain(ID3D11Device* device)
{
    back = new Sprite(device, L"./Data/Sprites/kariAsset.jpg");

    //model = new Model(device, ".\\Data\\Models\\Stage\\MDL_stage_ah.fbx", true, 0);
    model = new Model(device, "Data/Models/Player/T5.fbx", true, 0);
    scale.x = scale.y = scale.z = 0.05f;

    position.x = -140;
    position.y = -20;
    position.z = 5;

    angle.y = DirectX::XMConvertToRadians(-90);

    bgpos = { -200, -600 };

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageMain::~StageMain()
{
    delete model;
    delete back;
}

void StageMain::Update(float elapsedTime)
{
    bgpos = { player->GetPosition().x-20,player->GetPosition().y };
    UpdateTransform();
    model->UpdateTransform(transform);
}

void StageMain::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    //Scroll.data.scroll_direction;
    //deviceContext->UpdateSubresource(
    back->render(deviceContext, bgpos.x, -150, 1500, 1150, 1.0f, 1.0f, 1.0f, 1.0f, 0);

    model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    model->Render(deviceContext);

    // 必要なったら追加
    debugRenderer.get()->DrawSphere(position, 25, Vec4(1,0,1,1));

    //debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;
    //return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}