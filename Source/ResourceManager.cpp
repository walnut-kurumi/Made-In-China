#include "Graphics/Graphics.h"
#include "ResourceManager.h"
// ���f�����\�[�X�ǂݍ���
std::shared_ptr<SkinnedMesh> ResourceManager::LoadModelResource(const char* filename, bool triangulate, BOOL frontCounterClockwise) {
	// ���f��������
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())	{
		// �����N�i�����j���؂�Ă��Ȃ����m�F
		if (!it->second.expired()) {
			// �ǂݍ��ݍς݂̃��f�����\�[�X��Ԃ�
			return it->second.lock();
		}
	}
	// �V�K���f�����\�[�X�쐬���ǂݍ���
	ID3D11Device* device = Graphics::Ins().GetDevice();
	auto model = std::make_shared<SkinnedMesh>(device, filename, triangulate);
	model.get()->Init(device, frontCounterClockwise);

	//auto model = std::make_shared<SkinnedMesh>();
	//model->Load(device, filename);


	// �}�b�v�ɓo�^
	models[filename] = model;
	return model;
}

std::shared_ptr<SkinnedMesh::Animation> ResourceManager::LoadAnimationResource(const char* filename, float samplingRate, int index) {
	// ���f��������
	AnimeMap::iterator it = animes.find(filename);
	if (it != animes.end()) {
		// �����N�i�����j���؂�Ă��Ȃ����m�F
		if (!it->second.expired()) {
			// �ǂݍ��ݍς݂̃��f�����\�[�X��Ԃ�
			return it->second.lock();
		}
	}

	// �V�K���f�����\�[�X�쐬���ǂݍ���
	auto model = std::make_unique<SkinnedMesh>(filename, samplingRate, index);
	auto anime = std::make_shared<SkinnedMesh::Animation>(model->GetAnimes(index));
	// �}�b�v�ɓo�^
	animes[filename] = anime;
	return anime;

}