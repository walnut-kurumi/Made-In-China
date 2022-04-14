#pragma once
#pragma once
#include <memory>
#include <string>
#include <map>
#include "Graphics/SkinnedMesh.h"
// リソースマネージャー
class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}
public:
	// 唯一のインスタンス取得
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}
	// モデルリソース読み込み
	std::shared_ptr<SkinnedMesh> LoadModelResource(const char* filename, bool triangulate, BOOL frontCounterClockwise);

	std::shared_ptr<SkinnedMesh> LoadAnimationResource(const char* filename, float samplingRate, int index);
private:
	using ModelMap = std::map<std::string, std::weak_ptr<SkinnedMesh>>;
	ModelMap models;

	using AnimeMap = std::map<std::string, std::weak_ptr<SkinnedMesh>>;
	AnimeMap animes;
};