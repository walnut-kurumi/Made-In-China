#include "SBNormal.h"
#include "Camera/CameraManager.h"

SBNormal::SBNormal(ID3D11Device* device, SBManager* manager) 
	: SB(manager) {
	model = new Model(device, "Data/Models/Bullet/katana.fbx", true, 0);
	// 表示サイズを調整
	scale.x = scale.z = 0.05f;
	scale.y = 0.14f;

	debugRenderer = std::make_unique<DebugRenderer>(device);
}

SBNormal::~SBNormal() {
	delete model;
}
// 更新処理
void SBNormal::Update(float elapsedTime) {
	Vec3 m = position;
	// 寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f) {
		// 自分を削除
		Destroy();
	}
	// 移動
	bulletSpeed = this->speed * elapsedTime;
	position.x += direction.x * bulletSpeed;
	position.y += direction.y * bulletSpeed;
	position.z += direction.z * bulletSpeed;

	// 移動距離
	moveLen += position - m;
	// オブジェクト行列を更新
	UpdateTransform();
	// モデル行列更新
	model->UpdateTransform(transform);
}
// 描画処理
void SBNormal::Render(ID3D11DeviceContext* dc, Shader* shader) {
	model->Begin(dc, *shader);
	model->Render(dc);
#ifdef _DEBUG
	//// 必要なったら追加
	debugRenderer.get()->DrawSphere(position, radius, Vec4(1, 0.5f, 0, 1));

	//debugRenderer.get()->Render(dc, CameraManager::Instance().GetViewProjection());
#endif
}

// 発射処理
void SBNormal::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position) {
	this->direction = direction;
	this->position = position;
}
