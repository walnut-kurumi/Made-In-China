#pragma once

#include "Model.h"
#include "EnemyBullet.h"
#include "EnemyBulletManager.h"

// 直進弾丸
class EnemyBulletStraight : public EnemyBullet
{
public:
	EnemyBulletStraight(ID3D11Device* device, EnemyBulletManager* manager);
	~EnemyBulletStraight() override;

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader)override;
	// 発射
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	std::unique_ptr<Model> model = nullptr;
	float lifeTimer = 20.0f;

	// エフェクト
	//Effect* ballisticEffect = nullptr;
	//Effekseer::Handle handle = 0;
};
