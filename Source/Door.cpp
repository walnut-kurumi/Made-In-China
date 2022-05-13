#include "Door.h"
#include "Camera/CameraManager.h"

Door::Door(ID3D11Device* device)
{
	model = new Model(device, "./Data/Models/Stage/StageDoor.fbx", true, 0);

	scale.x = scale.y = scale.z = 0.05f;
    angle = { 0, DirectX::XMConvertToRadians(90), 0 };

    isOpen = false;    
    radius = 4.0f;

	debugRenderer = std::make_unique<DebugRenderer>(device);
}

Door::~Door()
{
	delete model;
}

void Door::Init()
{    
    scale.x = scale.y = scale.z = 0.05f;
    angle = { 0, DirectX::XMConvertToRadians(90), 0 };

    isOpen = false;
    radius = 4.0f;

    centerPos = position;
    centerPos.y = position.y + 5.0f;
    centerPos.z = 0.0f;
}

void Door::Update(float elapsedTime)
{
    centerPos = position;    
    centerPos.y = position.y + 5.0f;
    centerPos.z = 0.0f;    
    

	UpdateTransform();
    model->UpdateTransform(transform);


    // �v���C���[�̍U���ƃh�A�̓����蔻��
    CollisionPlayerAtkVsDoor();

    // �h�A�J����
    OpenTheDoor();
}

void Door::Render(ID3D11DeviceContext* dc, float elapsedTime)
{
	model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
	model->Render(dc);

	// �K�v�Ȃ�����ǉ�
    debugRenderer.get()->DrawSphere(centerPos, radius, Vec4(1, 0, 0, 1));
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
}

void Door::RenderGui()
{
#ifdef USE_IMGUI   
    if (ImGui::Begin("Door", nullptr, ImGuiWindowFlags_None)) {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat3("Pos", &position.x, -100, 100);
            ImGui::SliderFloat3("centerPos", &centerPos.x, -100, 100);
            ImGui::SliderFloat3("Angle", &angle.x, 0, 3.14f);
        }
        unsigned int flag = static_cast<int>(isOpen);
        ImGui::CheckboxFlags("IsOpen", &flag, 1);      
    }
    ImGui::End();
#endif
}

bool Door::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::RayVsModel(start, end, model->GetSkinnedMeshs(), transform, hit);
}

// �U�����ꂽ��h�A�J����
void Door::OpenTheDoor()
{
    if (isOpen && (angle.y <= 3.14 && angle.y >= 0))
    {
        Vec3 p = player->GetPosition();
        Vec3 d = centerPos;
        Vec3 pd = VecMath::Normalize(d - p);

        if (pd.x > 0)angle.y += DirectX::XMConvertToRadians(15.0f);
        else angle.y -= DirectX::XMConvertToRadians(15.0f);
    }
}

void Door::CollisionPlayerAtkVsDoor()
{
    if (player->GetIsAtk())
    {        

        if (Collision::SphereVsSphere(centerPos, player->GetAttackPosistion(), radius, player->GetAtkRadius()))
        {
            isOpen = true;
        }
    }

}

void Door::UpdateTransform()
{

    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    // ��]�s����쐬
    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;

    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // 3�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;

    // �v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}