#include "StageMain.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageMain::StageMain(ID3D11Device* device)
{
    back = new Sprite(device, L"./Data/Sprites/kariAsset.jpg");

    model = new Model(device, ".\\Data\\Models\\Stage\\stage.fbx",true,0);
    scale.x = scale.y = scale.z = 0.05f;
   
    angle.y = DirectX::XMConvertToRadians(-90);

    type = Type::Main;
    debugRenderer = std::make_unique<DebugRenderer>(device);
}

StageMain::~StageMain()
{
    delete model;    
}

void StageMain::Update(float elapsedTime)
{
    UpdateTransform();    
    model->UpdateTransform(transform);   
}

void StageMain::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{         

    back->render(deviceContext, 0, 0, 3840, 2160, 1.0f, 1.0f, 1.0f, 1.0f, 0);

    model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    model->Render(deviceContext);  

    // �K�v�Ȃ�����ǉ�
    debugRenderer.get()->DrawSphere(position, 25, Vec4(1,0,1,1));

    //debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    //return false;
    return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}