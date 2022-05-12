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

	// 必要なったら追加
	//debugRenderer.get()->DrawSphere(position, 25, Vec4(1, 0, 1, 1));
}

void Door::RenderGui()
{
#ifdef USE_IMGUI   
    if (ImGui::Begin("Door", nullptr, ImGuiWindowFlags_None)) {
        // トランスフォーム
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

// 攻撃されたらドア開ける
void Door::OpenTheDoor()
{
    //isOpen = true;
}

void Door::UpdateTransform()
{

    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    // 回転行列を作成
    //DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;

    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // 3つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;

    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}
