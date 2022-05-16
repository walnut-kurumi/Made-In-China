#pragma once

#include "Stage.h"
#include "Collision.h"

// コピーコンストラクタ防止マクロ
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

class StageManager
{
private:
	StageManager() = default;
	~StageManager() = default;
	static StageManager* instance;

public:
	// クラスのprivate:に置くことでデフォルトのコピーコンストラクタ
	// 代入演算子を生成させないマクロ Readable codeから。
	DISALLOW_COPY_AND_ASSIGN(StageManager);

	static StageManager& Instance()	{
		return *instance;
	}

	static void Create() {
		if (!instance) instance = new StageManager;
	}

	static void Destory() {
		delete instance;
		instance = nullptr;
	}

	void Init();

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc, float elapsedTime);

	void RenderGui();

	void Register(Stage* stage);

	void Clear();

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	int GetStageCount()const { return static_cast<int>(stages.size()); }
	Stage* GetStage(int index) { return stages.at(index); }

	// プレイヤーと次のステージへ移るときの場所で判定
	bool CollisionPlayerVsNextStagePos(Vec3 playerPos, float playerRadius);

private:
	std::vector<Stage*> stages;

};