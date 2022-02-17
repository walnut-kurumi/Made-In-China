#include "StageSkybox.h"
#include "StageManager.h"
#include "Graphics/Shaders.h"

StageSkybox::StageSkybox(ID3D11Device* device)
{
      
    model = new Model(device, ".\\Data\\Models\\Stage\\skybox\\skybox.fbx", true, 0.0f, true);

    scale = { 6.5f,6.5f,6.5f };   

    type = Type::Normal;
}

StageSkybox::~StageSkybox()
{
    delete model;
}

void StageSkybox::Update(float elapsedTime)
{
    UpdateTransform();
    model->UpdateTransform(transform);    
}

void StageSkybox::Render(ID3D11DeviceContext* deviceContext, float elapsedTime)
{
    model->Begin(deviceContext, Shaders::Ins()->GetSkyboxShader());    
    model->Render(deviceContext);
}

bool StageSkybox::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;
}