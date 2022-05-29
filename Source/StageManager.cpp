#include "StageManager.h"

StageManager* StageManager::instance{};

void StageManager::Init()
{

}

void StageManager::Update(float elapsedTime)
{
    for (Stage* stage : stages) stage->Update(elapsedTime);
}

void StageManager::Render(ID3D11DeviceContext* dc, float elapsedTime)
{
    for (Stage* stage : stages) stage->Render(dc, elapsedTime);
}

void StageManager::RenderGui()
{
    for (Stage* stage : stages) stage->RenderGui();
}

void StageManager::Register(Stage* stage)
{
    stages.emplace_back(stage);
}

void StageManager::Clear()
{
    for (Stage* stage : stages) delete stage;
    stages.clear();
}

bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    hit.distance = FLT_MAX;
    bool normal = false;

    for (Stage* stage : stages) {
        HitResult tmp;
        if (stage->RayCast(start, end, tmp)) {
            if (tmp.distance < hit.distance) {
                hit = tmp;
                // タイプが貫通じゃなかったら
                if (stage->GetType() != Stage::Type::Penetrate) normal = true;
                // タイプが貫通だったら
                else hit.penetrate = true;
                result = true;
            }
        }
    }
    if (normal) hit.penetrate = false;
    return result;
}

// プレイヤーと次のステージへ移るときの場所で判定
bool StageManager::CollisionPlayerVsNextStagePos(Vec3 playerPos, float playerRadius)
{
    for (Stage* stage : stages) {        
        if (stage->GetType() == Stage::Type::Main)
        {
            return stage->CollisionPlayerVsNextStagePos(playerPos, playerRadius);
        }
    }
    return false;
}
