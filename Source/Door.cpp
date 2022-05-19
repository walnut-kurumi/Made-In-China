#include "Door.h"
#include "Camera/CameraManager.h"
#include "EnemyManager.h"

Door::Door(ID3D11Device* device)
{
	model = new Model(device, "./Data/Models/Stage/StageDoor.fbx", true, 0);
    backModel = new Model(device, "./Data/Models/Stage/StageDoor.fbx", true, 0);

	scale.x = scale.y = scale.z = 0.05f;
    angle = { 0, DirectX::XMConvertToRadians(90), 0 };

    isOpen = false;
    radius = 4.0f;
    radian = 0.0f;
    radian2 = 3.1415f * 0.5f;
    isBlinking = false;
    backModelAlpha = 0.0f;
    modelAlpha = 1.0f;

	debugRenderer = std::make_unique<DebugRenderer>(device);
}

Door::~Door()
{
    delete backModel;
	delete model;
}

void Door::Init()
{    
    scale.x = scale.y = scale.z = 0.05f;
    angle = { 0, DirectX::XMConvertToRadians(90), 0 };

    isOpen = false;
    radius = 4.0f;
    radian = 0.0f;
    radian2 = 3.1415f * 0.5f;
    isBlinking = false;
    backModelAlpha = 0.0f;
    modelAlpha = 1.0f;

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
    backModel->UpdateTransform(transform);


    // プレイヤーの攻撃とドアの当たり判定
    CollisionPlayerAtkVsDoor();
    // プレイヤーとドア
    CollisionPlayerVsDoor();
    // ドア開ける
    OpenTheDoor();

    // 点滅
    if (!isOpen)
    {
        radian += 0.05f;
        radian2+= 0.05f;
        if (radian > 3.1415f) radian = 0.0f;
        if (radian2 > 3.1415f) radian2 = 0.0f;
        if (isBlinking)
        {
            backModelAlpha = sinf(radian);
            modelAlpha = sinf(radian2);            
        }
        backModelColor.w = backModelAlpha;
    }
    if (!isBlinking)
    {
        backModelColor.w = 1.0f;
        modelAlpha = 1.0f;
    }
}

void Door::Render(ID3D11DeviceContext* dc, float elapsedTime)
{
    if (!isOpen && isBlinking)
    {
        backModel->Begin(dc, Shaders::Ins()->GetOutlineShader(), true);        
        backModel->Render(dc, backModelColor);
    }
    model->Begin(dc, Shaders::Ins()->GetSkinnedMeshShader());
    Vec4 color = { 0.564f,0.42f,0.1f,modelAlpha };    
    model->Render(dc,color);
	

#ifdef _DEBUG
    debugRenderer.get()->DrawSphere(centerPos, radius, Vec4(1, 0, 0, 1));
    debugRenderer.get()->DrawSphere(centerPos, backModelRadius, Vec4(0.3f, 1, 0, 1));    
    debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());

#endif // _DEBUG

}

void Door::RenderGui()
{
#ifdef USE_IMGUI   
    if (ImGui::Begin("Door", nullptr, ImGuiWindowFlags_None)) {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat3("Pos", &position.x, -200, 200);            
            ImGui::SliderFloat3("Angle", &angle.x, 0, 3.14f);            
            ImGui::SliderFloat4("backmdl", &backModelColor.x,0.0f,1.0f);
            ImGui::SliderFloat("mdl", &modelAlpha,0.0f,1.0f);
        }
        unsigned int flag = static_cast<int>(isOpen);
        unsigned int flag2 = static_cast<int>(isBlinking);
        ImGui::CheckboxFlags("IsOpen", &flag, 1);      
        ImGui::CheckboxFlags("IsBlink", &flag2, 1);      
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
    if (isOpen && (angle.y <= 3.14 && angle.y >= 0))
    {
        Vec3 p = player->GetPosition();
        Vec3 d = centerPos;
        Vec3 pd = VecMath::Normalize(d - p);

        // Right
        if (pd.x > 0)
        {
            angle.y += DirectX::XMConvertToRadians(15.0f);

            collisionPos = centerPos;
            collisionPos.x = centerPos.x + 4.0f;
        }
        // Left
        else
        {
            angle.y -= DirectX::XMConvertToRadians(15.0f);

            collisionPos = centerPos;
            collisionPos.x = centerPos.x - 4.0f;
        }

        // 敵とドアの判定
        CollisionEnemyVsDoor();

        modelAlpha = 1.0f;
    }
}

// 自機攻撃とドアの判定
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

// 自機とドア
void Door::CollisionPlayerVsDoor()
{
    if (isOpen) return;
    if (Collision::SphereVsSphere(centerPos, player->GetCenterPosition(), backModelRadius, player->GetRadius()))
    {
        // 点滅する
        isBlinking = true;
    }
    else isBlinking = false;
}

// ドアを開けたときに敵が重なっていたら倒す
void Door::CollisionEnemyVsDoor()
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        // 死んでたらリターン
        if (enemy->GetHealth() <= 0) return;

        // 衝突処理
        if (Collision::SphereVsSphere(enemy->GetPosition(), collisionPos, enemy->GetRadius(), radius))
        {
            enemy->ApplyDamage(1, 0);
            // ヒットストップ
            if (!player->GetSlowFlag())player->SetHitstop(true);
            // カメラシェイク（簡素）
            CameraManager& cameraMgr = CameraManager::Instance();
            if (!cameraMgr.GetShakeFlag())
            {
                cameraMgr.SetShakeFlag(true);;
            }
        }
    }
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
