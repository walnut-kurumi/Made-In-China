#pragma once

#include "Stage.h"
#include "Graphics/Sprite.h"
#include "Graphics/Vec.h"
#include "Framework.h"

class Goal
{
	Goal() {};
	~Goal();

	static Goal* instance;

public:
	// インスタンス取得
	static Goal& Instance()
	{
		static Goal instance;
		return instance;
	}
	void Init(ID3D11Device* device);

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);

	void SetGoalPos(Vec3 g) { goalPos = g; }
	void SetPlayerPos(Vec3 p) { playerPos = p; }
	void SetCanGoal(bool g) { canGoal = g; }

private:	
	Sprite* goalArrow{};

	float arrowAngle = 0;
	Vec3 goalPos = { 0,0,0 };
	Vec3 playerPos = { 0,0,0 };
	bool canGoal = false;

	float radian = 0.0f;
	float movePosY = 0.0f;
};
