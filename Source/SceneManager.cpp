#include "SceneManager.h"

// ������ 
void SceneManager::Init()
{
}

//�@�X�V
void SceneManager::Update(float elapsedTime)
{
    if (currentScene) currentScene->Update(elapsedTime);
}

// �`��
void SceneManager::Render(float elapsedTime)
{
    if (currentScene) currentScene->Render(elapsedTime);
}

// �N���A
void SceneManager::Clear()
{
    if (currentScene) {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

void SceneManager::ChangeScene(Scene* scene)
{
    // �Â��V�[�����I������
    Clear();
    // �V�����V�[����ݒ�
    currentScene = scene;
    // �V�[��������
    if (!currentScene->Ready()) currentScene->Initialize();
}