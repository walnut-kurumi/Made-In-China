#pragma once
#include <DirectXMath.h>
#include "Camera.h"
#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/Vec.h"

class CameraManager
{
private:
	CameraManager() = default;
	~CameraManager() = default;
	inline static CameraManager* instance;
public:

	static CameraManager& Instance() // シングルトン
	{
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new CameraManager;
	}

	static void Destroy() {
		delete instance;
		instance = nullptr;
	}


	void Init();
	void Update(float elapsedTime);
	void SetBuffer(ID3D11DeviceContext* dc);

	void SetTarget(const Vec3& target) { this->target = target; }
	void SetGoal(const Vec3& goal) { this->goal = goal; }

	bool CameraRay();
	// カメラ追跡遅延
	void CameraDelay();
	
	DirectX::XMFLOAT4X4 GetViewProjection() { return viewProjection; }
	DirectX::XMFLOAT4X4 GetView() { return mainC.GetView(); }
	DirectX::XMFLOAT4X4 GetProjection() { return mainC.GetProjection(); }
	Vec3 GetPos() { return position; }
	Vec3 GetTarget() { return target; }
	Vec3 GetGoal() { return goal; }

	void SetShakeFlag(bool s) { shake = s; shakeTimer = 0.2f; }
	bool GetShakeFlag() { return shake; }
	void UpdateShakeTimer(float elapsedTime);

	void UpdateTimer();
private:

	Vec3 target = { 0,0,0 };
	Vec3 goal = { 0,0,0 };
	Vec3 angle = { 0,0,0 };
	Vec3 position = { 0,0,0 };
	DirectX::XMFLOAT4X4 viewProjection;

	Vec3 camShake = { 0,0,0 };
	bool shake = false;
	float shakeTimer = 0.0f;

	float rollSpeed = 1.0f;
	float range = 145.0f; // 元65.0f

	float maxAngle = DirectX::XMConvertToRadians(45);
	float minAngle = DirectX::XMConvertToRadians(-45);

	ConstantBuffer<CbScene> cbScene;

	int timer = 0;

	float delay = 0.1f;
public:

	Camera mainC{};
};