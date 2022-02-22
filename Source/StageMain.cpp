#include "StageMain.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"
#include "Camera/CameraManager.h"
#include "Input/GamePad.h"
#include "Input/input.h"

StageMain::StageMain(ID3D11Device* device)
{


    model = new Model(device, ".\\Data\\Models\\Stage\\stage_1.fbx",true,0);
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
    model->Begin(deviceContext, Shaders::Ins()->GetRampShader());
    model->Render(deviceContext);  

    // •K—v‚È‚Á‚½‚ç’Ç‰Á
    debugRenderer.get()->DrawSphere(position, 25, Vec4(1,0,1,1));

    debugRenderer.get()->Render(deviceContext, CameraManager::Instance().GetViewProjection());
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    //return false;
    return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}