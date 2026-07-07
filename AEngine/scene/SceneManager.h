#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include <vector>
#include "../core/AEngineAPI.h"

class Scene;
class GameObject;

class SceneManager
{
public:
	AENGINE_API static SceneManager& Get();
	AENGINE_API Scene& GetActiveScene();
	AENGINE_API void LoadScene(int count);
	AENGINE_API void CreateScene();
	AENGINE_API GameObject* CreateGameObject();

	AENGINE_API ~SceneManager();

private:

	Scene* activeScene = nullptr;
	std::vector<std::unique_ptr<Scene>> scenes;
};

#endif
