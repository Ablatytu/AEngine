#pragma once
#include <string>

class Scene;

class SceneSerializer
{
public:
	void SaveScene(Scene& scene);
	void LoadScene(std::string path);
	void CreateScene(std::string name);
};