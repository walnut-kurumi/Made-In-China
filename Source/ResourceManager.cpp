#include "Graphics/Graphics.h"
#include "ResourceManager.h"
// モデルリソース読み込み
std::shared_ptr<SkinnedMesh> ResourceManager::LoadModelResource(const char* filename, bool triangulate, BOOL frontCounterClockwise)
{
	// モデルを検索
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())	{
		// リンク（寿命）が切れていないか確認
		if (!it->second.expired()) {
			// 読み込み済みのモデルリソースを返す
			return it->second.lock();
		}
	}
	// 新規モデルリソース作成＆読み込み
	ID3D11Device* device = Graphics::Ins().GetDevice();
	auto model = std::make_shared<SkinnedMesh>(device, filename, triangulate);
	model.get()->Init(device, frontCounterClockwise);

	//auto model = std::make_shared<SkinnedMesh>();
	//model->Load(device, filename);


	// マップに登録
	models[filename] = model;
	return model;
}