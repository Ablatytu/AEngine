#pragma once
#include <vector>
#include <memory>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

class GameObject;
class AComponent;
class Render;
class Camera;
class Skybox;
class Light;
class RenderContext;
class LightManager;
class PostProcess;

class Scene
{
	public:
	std::vector<std::unique_ptr<GameObject>> objects;
	std::vector<AComponent*> AComponents;
	std::vector<AComponent*> AwakeAComponents;
	std::vector<AComponent*> StartAComponents;
	std::vector<Render*> renders;
	PostProcess* postProcess = nullptr;
	struct FogSettings
	{
		bool Fog = false;
		glm::vec3 FogColor = glm::vec3(1.0f);
	};
	FogSettings fogSettings;
	bool HDR = false;
	Camera* activeCamera = nullptr;
	GameObject* CreateGameObject();
	RenderContext CollectRenderContext();
	std::unique_ptr<Skybox> SkyBox;
	std::unique_ptr<LightManager> A_LightManager;
	Scene();
	~Scene();
};