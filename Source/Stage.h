#pragma once

#include "Model.h"
#include "Collision.h"

class Stage
{
public:
	enum class Type
	{
		Normal,	// 特別なことはない
		Main,	// メインステージ
		Penetrate, // 貫通床
		End
	};

public:
	Stage() {}
	~Stage() {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(ID3D11DeviceContext* deviceContext, float elapsedTime) = 0;
	virtual void RenderGui() {}

	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	const Vec3& GetPos() const { return position; }
	void SetPos(const Vec3& position) { this->position = position; }
	const Vec3& GetAngle() const { return angle; }
	void SetAngle(const Vec3& angle) { this->angle = angle; }
	const Vec3& GetScale() const { return scale; }
	void SetScale(const Vec3& scale) { this->scale = scale; }


	virtual void SetAngle(Vec3 ang) { angle = ang; }

	Type GetType() { return type; }
	void SetType(Type ty) { type = ty; }

protected:
	// 行列更新処理
	void UpdateTransform();

protected:
	Model* model;

	Vec3 position = { 0, 0, 0 };
	Vec3 angle = { 0, 0 ,0 };
	Vec3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	Type type = Type::Normal;


};