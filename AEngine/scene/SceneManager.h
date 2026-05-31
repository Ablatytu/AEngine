#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include <vector>

class Scene;
class GameObject;

class SceneManager
{
public:
	static SceneManager& Get();
	Scene& GetActiveScene();
	void LoadScene(int count);
	void CreateScene();
	GameObject* CreateGameObject();

	~SceneManager();

private:

	Scene* activeScene = nullptr;
	std::vector<std::unique_ptr<Scene>> scenes;
};

#endif
