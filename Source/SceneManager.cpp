#include "SceneManager.h"

// 初期化 
void SceneManager::Init()
{
}

//　更新
void SceneManager::Update(float elapsedTime)
{
    if (currentScene) currentScene->Update(elapsedTime);
}

// 描画
void SceneManager::Render(float elapsedTime)
{
    if (currentScene) currentScene->Render(elapsedTime);
}

// クリア
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
    // 古いシーンを終了処理
    Clear();
    // 新しいシーンを設定
    currentScene = scene;
    // シーン初期化
    if (!currentScene->Ready()) currentScene->Initialize();
}