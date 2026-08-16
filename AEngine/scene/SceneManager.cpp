#include "SceneManager.h"
#include "../core/ErrorManager.h"
#include "Scene.h"
#include "../Renderer/GameObject.h"
#include "SceneSerializer.h"

SceneManager& SceneManager::Get()
{
	static SceneManager manager;
	return manager;
}
//void SceneManager::LoadScene(int count)
//{
//	if(count < scenes.size())
//		activeScene = scenes[count].get();
//	Application::Get().OnSceneLoaded();
//}
Scene* SceneManager::SwapActiveScene(Scene* newScene)
{
	Scene* oldScene = activeScene;
	activeScene = newScene;
	return oldScene;
}
void SceneManager::ChangeScene()
{
	if (ChangeScenePath.empty()) return;
	if (activeScene)
	{
		activeScene->EraseScene();
		scenes.clear();
		activeScene = nullptr;
	}
	std::unique_ptr newScene = SceneSerializer::Deserialize(ChangeScenePath, FromString);
	if (newScene)
	{
		newScene->pathScene = ScenePathAfterLoad;
		scenes.push_back(std::move(newScene));
		activeScene = scenes.back().get();
	}
	else
		std::cout << "Scene not found!\n";
	ChangeScenePath = "";
	FromString = false;
}
bool SceneManager::CheckChangeScenePathState()
{
	if (!ChangeScenePath.empty())
		return true;
	return false;
}
void SceneManager::LoadScene(const std::string& path)
{
	ChangeScenePath = path;
	ScenePathAfterLoad = path;
	FromString = false;
}
void SceneManager::LoadSceneFromString(const std::string& data, const std::string& originalPath)
{
	ChangeScenePath = data;
	ScenePathAfterLoad = originalPath;
	FromString = true;
}
void SceneManager::CreateScene(const std::string& path)
{
	SceneSerializer::SerializeEmpty(path);
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