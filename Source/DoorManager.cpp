#include "DoorManager.h"

void DoorManager::Init()
{
    for (Door* door : doors) door->Init();
}

void DoorManager::Update(float elapsedTime)
{
	for (Door* door : doors) door->Update(elapsedTime);
}

void DoorManager::Render(ID3D11DeviceContext* dc, float elapsedTime)
{
	for (Door* door : doors) door->Render(dc,elapsedTime);

    RenderGui();
}

void DoorManager::RenderGui()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);    
#endif
    for (Door* door : doors) door->RenderGui();
}

void DoorManager::Register(Door* door)
{
	doors.emplace_back(door);
}

void DoorManager::Clear()
{
	for (Door* door : doors)delete door;
	doors.clear();
}

bool DoorManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    hit.distance = FLT_MAX;

    for (Door* door : doors) {
        HitResult tmp;
        if (door->RayCast(start, end, tmp)) 
        {
            if (tmp.distance < hit.distance) 
            {
                hit = tmp;                
                result = true;
            }
        }
    }

    return result;
}
