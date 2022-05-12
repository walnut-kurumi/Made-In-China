#pragma once

#include "Door.h"
#include "Collision.h"

class DoorManager
{
private:
	DoorManager() = default;
	~DoorManager() = default;
	static DoorManager* instance;

public:
	// 唯一のインスタンス取得
	static DoorManager& Instance()
	{
		static DoorManager instance;
		return instance;
	}
	void Init();

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc, float elapsedTime);

	void RenderGui();

	void Register(Door* door);

	void Clear();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	int GetDoorCount()const { return static_cast<int>(doors.size()); }
	Door* GetDoor(int index) { return doors.at(index); }

private:
	std::vector<Door*> doors;

};
