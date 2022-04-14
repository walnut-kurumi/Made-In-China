#pragma once
#pragma once
#include <memory>
#include <string>
#include <map>
#include "Graphics/SkinnedMesh.h"
// ���\�[�X�}�l�[�W���[
class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}
public:
	// �B��̃C���X�^���X�擾
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}
	// ���f�����\�[�X�ǂݍ���
	std::shared_ptr<SkinnedMesh> LoadModelResource(const char* filename, bool triangulate, BOOL frontCounterClockwise);

	std::shared_ptr<SkinnedMesh> LoadAnimationResource(const char* filename, float samplingRate, int index);
private:
	using ModelMap = std::map<std::string, std::weak_ptr<SkinnedMesh>>;
	ModelMap models;

	using AnimeMap = std::map<std::string, std::weak_ptr<SkinnedMesh>>;
	AnimeMap animes;
};