#pragma once

#include <vector>
#include "Enemy.h"

// エネミーマネージャー
class EnemyManager
{
private:
	enum class State
	{
		Idle,		// 待機 徘徊ターン時に少し挟む
		Run,		// 接敵したら射程距離に入るまで
		Walk,		// 徘徊 基本これ
		Attack,		// 射程距離に入ったら死ぬか射程距離から離れるまで攻撃
		Blow,		// 致死ダメージを受けたときに吹っ飛ばすときの姿勢
		Death,		// 吹っ飛ばされたあとの姿勢
		End,		// お　わ　り
	};

private:
	EnemyManager() {}
	~EnemyManager() { delete model; }
public:
	// 唯一のインスタンス取得
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}	
	// 初期化処理
	void Init();
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);	
	// エネミー登録
	void Register(Enemy* enemy);
	// エネミー削除
	void Remove(Enemy* enemy);
	// エネミー全削除
	void Clear();	
	// エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }
	// エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }
	// ソート
	void SortLengthSq(const DirectX::XMFLOAT3 playerPos);

	void SetPlayerPos(Vec2 playerPos);

private:
	// エネミー同士の衝突処理
	void CollisionEnemyVsEnemies();
	// 複数のエネミーを管理するため、ポインタをvectorで管理する
	std::vector<Enemy*> enemies;
	std::vector<Enemy*> removes;	

	Model* model = nullptr;
};
