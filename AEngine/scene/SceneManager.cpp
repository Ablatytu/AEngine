#include "SceneManager.h"
#include "../core/ErrorManager.h"
#include "Scene.h"
#include "../Renderer/GameObject.h"

SceneManager& SceneManager::Get()
{
	static SceneManager manager;
	return manager;
}
void SceneManager::LoadScene(int count)
{
	if(count < scenes.size())
		activeScene = scenes[count].get();
}
void SceneManager::CreateScene()
{
	std::unique_ptr<Scene> newScene = std::make_unique<Scene>();
	scenes.push_back(std::move(newScene));
}
Scene& SceneManager::GetActiveScene()
{
	if (!activeScene)
		CriticalGameError("No active scene");
	return *activeScene;
}
GameObject* SceneManager::CreateGameObject()
{
	return GetActiveScene().CreateGameObject();
}

SceneManager::~SceneManager() = default;