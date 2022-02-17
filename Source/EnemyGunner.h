//#pragma once
//
//#include "Model.h"
//#include "Enemy.h"
//
//// Gunner
//class EnemyGunner : public Enemy
//{
//private:
//
//	enum class State
//	{
//		Idle,		// 待機 徘徊ターン時に少し挟む
//		Run,		// 接敵したら射程距離に入るまで
//		Walk,		// 徘徊 基本これ
//		Attack,		// 射程距離に入ったら死ぬか射程距離から離れるまで攻撃
//		Blow,		// 致死ダメージを受けたときに吹っ飛ばすときの姿勢
//		Death,		// 吹っ飛ばされたあとの姿勢
//		End,		// お　わ　り
//	};
//
//public:
//	EnemyGunner(ID3D11Device* device);
//	~EnemyGunner()override;
//
//	void Init();
//	void Update(float elapsedTime);
//
//	void Render(ID3D11DeviceContext* dc);
//
//	void DrawDebugGUI();
//	
//	bool GetDead() { return isDead; }
//
//private:	
//	// 徘徊  ←左true　false右→
//	void MoveWalk(bool direction );
//	// 索敵　引っかかったらtrue返す
//	bool Search();
//	// 接敵
//	void MoveRun();
//	// 射撃攻撃
//	void MoveAttack();
//	// 吹っ飛び
//
//	
//	// 死亡した時に呼ばれる
//	void OnDead() override { isDead = true; }
//
//protected:
//
//	// 状態遷移
//	// 待機ステート
//	void TransitionIdleState();
//	void UpdateIdleState(float elapsedTime);
//
//	// 移動ステート
//	void TransitionWalkState();
//	void UpdateWalkState(float elapsedTime);
//
//	// 走るステート
//	void TransitionRunState();
//	void UpdateRunState(float elapsedTime);
//
//	// 攻撃ステート
//	void TransitionAttackState();
//	void UpdateAttackState(float elapsedTime);
//
//	// 吹っ飛びステート
//	void TransitionBlowState();
//	void UpdateBlowState(float elapsedTime);
//
//	// 死亡ステート
//	void TransitionDeathState();
//	void UpdateDeathState(float elapsedTime);
//
//
//private:
//	template<class Type, typename Return, typename ...Args>
//	using Temp = Return(Type::*)(Args...);
//	Temp<EnemyGunner, void, float> UpdateState[static_cast<int>(State::End)];
//
//	State state = State::Idle;
//
//private:
//	float moveSpeed = 5.0f;
//	float turnSpeed = DirectX::XMConvertToRadians(720);
//			
//	SkinnedMesh* skinned;
//
//	bool isDead = false;
//};
