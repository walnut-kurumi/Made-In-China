#pragma once

#include "Door.h"
#include "Collision.h"

// コピーコンストラクタ防止マクロ
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

class DoorManager
{
private:
	DoorManager() = default;
	~DoorManager() = default;
	static DoorManager* instance;

public:
	// クラスのprivate:に置くことでデフォルトのコピーコンストラクタ
	// 代入演算子を生成させないマクロ Readable codeから。
	DISALLOW_COPY_AND_ASSIGN(DoorManager);

	static DoorManager& Instance() {
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new DoorManager;
	}

	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	void Init();

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc, float elapsedTime);

	void RenderGui();

	void Register(Door* door);

	void Clear();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	int GetStageCount()const { return static_cast<int>(doors.size()); }
	Door* GetStage(int index) { return doors.at(index); }

private:
	std::vector<Door*> doors;

};
