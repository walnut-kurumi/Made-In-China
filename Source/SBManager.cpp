#include "SBManager.h"

SBManager::~SBManager() {
	Clear();
}

// 更新処理
void SBManager::Update(float elapsedTime) {
	// 更新処理
	for (SB* projectile : projectiles) {
		projectile->Update(elapsedTime);
	}
	// 破棄処理
	// ※projectilesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (SB* projectile : removes) {
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<SB*>::iterator it =
			std::find(projectiles.begin(), projectiles.end(), projectile);
		if (it != projectiles.end()) {
			projectiles.erase(it);
		}
		// 弾丸の破棄処理
		if (projectile) {
			projectile = nullptr;
			delete projectile;
		}
	}
	// 破棄リストをクリア
	removes.clear();
}
// 描画処理
void SBManager::Render(ID3D11DeviceContext* dc, Shader* shader) {
	for (SB* projectile : projectiles)	{
		projectile->Render(dc, shader);
	}
}

// 弾丸登録
void SBManager::Register(SB* projectile) {
	projectiles.push_back(projectile);
}
// 弾丸全削除
void SBManager::Clear() {
	for (SB* projectile : projectiles)	{
		delete projectile;
	}
	projectiles.clear();
}

// 弾丸削除
void SBManager::Remove(SB* projectile) {
	// 破棄リストに追加
	removes.emplace_back(projectile);
}
