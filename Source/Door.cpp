#include "Door.h"

Door::Door(ID3D11Device* device)
{
	model = new Model(device, "./Data/Models/Stage/StageDoor.fbx", true, 0);

	scale.x = scale.y = scale.z = 0.05f;

	debugRenderer = std::make_unique<DebugRenderer>(device);
}

Door::~Door()
{
	delete model;
}

void Door::Update(float elapsedTime)
{
	UpdateTransform();
}

void Door::Render(ID3D11DeviceContext* dc, float elapsedTime)
{
	model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
	model->Render(dc);

	// �K�v�Ȃ�����ǉ�
	//debugRenderer.get()->DrawSphere(position, 25, Vec4(1, 0, 1, 1));
}

void Door::RenderGui()
{
#ifdef USE_IMGUI   
    if (ImGui::Begin("Door", nullptr, ImGuiWindowFlags_None)) {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat3("Position", &position.x, -300, 300);
            ImGui::SliderFloat3("Angle", &angle.x, 0, 360);
        }
        unsigned int flag = static_cast<int>(isOpen);
        ImGui::CheckboxFlags("IsOpen", &flag, 0);      
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
    //isOpen = true;
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
