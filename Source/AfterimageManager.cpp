#include "AfterimageManager.h"

void AfterimageManager::Initialise() {
	pool.Create(preLoadCount);
}

void AfterimageManager::Update(float elapsedTime) {
	createTimer += elapsedTime;
	if (createTimer >= createIntervalTime) {
		createTimer = 0.0f;
		// –¢Žg—p‚Ìƒƒ‚ƒŠ‚ª‚ ‚ê‚ÎŽ‚Á‚Ä‚­‚é
		AfterimageSkinnedMesh* image = pool.Recycle();
		if (image != nullptr) {
			image->Init();
			image->SetParentData(meshes,materials,cb,transform);
		}
	}


	pool.Update(elapsedTime);
}

void AfterimageManager::Render(ID3D11DeviceContext* dc) {
	pool.Render(dc);
}

void AfterimageManager::Destroy() {
	pool.Destroy();
}

void AfterimageManager::SetParentData(
	std::vector<SkinnedMesh::Mesh> meshes, 
	std::unordered_map<uint64_t, SkinnedMesh::Material> materials, 
	SkinnedMesh::Constants cb, 
	DirectX::XMFLOAT4X4 transform
) {
	this->meshes = meshes;
	this->materials = materials;
	this->cb = cb;
	this->transform = transform;
}
