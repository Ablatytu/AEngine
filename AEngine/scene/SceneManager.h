#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include <vector>
#include "../core/AEngineAPI.h"
#include <string>
class Scene;
class GameObject;

class SceneManager
{
public:
	Scene* SwapActiveScene(Scene* newScene);
	AENGINE_API static SceneManager& Get();
	AENGINE_API Scene& GetActiveScene();
	AENGINE_API void LoadScene(int count);
	AENGINE_API void CreateScene(const std::string& path);
	AENGINE_API GameObject* CreateGameObject();
	AENGINE_API void LoadScene(const std::string& path);
	AENGINE_API void ChangeScene();
	AENGINE_API bool CheckChangeScenePathState();
	AENGINE_API ~SceneManager();

private:
	std::string ChangeScenePath = "";
	Scene* activeScene = nullptr;
	std::vector<std::unique_ptr<Scene>> scenes;
};

#endif
