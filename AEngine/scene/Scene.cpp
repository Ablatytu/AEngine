#include "Scene.h"
#include <memory>
#include "../Renderer/GameObject.h"
#include "Skybox.h"
#include "../Renderer/Cubemap.h"
#include "AssetManager.h"
#include "../Renderer/RenderContext.h"
#include "LightManager.h"
#include "../components/PostProcess.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

RenderContext Scene::CollectRenderContext()
{
	RenderContext context;
	context.camera = activeCamera;
	context.fog.enabled = fogSettings.Fog;
	context.fog.fogColor = fogSettings.FogColor;
	context.skybox = SkyBox.get();
	context.lights = A_LightManager->GetLights();
	context.renders = renders;
	context.HDR = HDR;
	context.postProcess = postProcess;
	return context;
}
GameObject* Scene::CreateGameObject()
{
	std::unique_ptr<GameObject> obj = std::make_unique<GameObject>();
	GameObject* objlink = obj.get();
	objects.push_back(std::move(obj));
	return objlink;
}
Scene::Scene()
{
	A_LightManager = std::make_unique<LightManager>();
	Cubemap* defaultSkybox = AssetManager::LoadCubemap("defaultSkyBox", 2048);
	Texture* textureRight = AssetManager::LoadTexture("Assets/Textures/skybox/right.jpg");
	Texture* textureLeft = AssetManager::LoadTexture("Assets/Textures/skybox/left.jpg");
	Texture* textureTop = AssetManager::LoadTexture("Assets/Textures/skybox/top.jpg");
	Texture* texturebottom = AssetManager::LoadTexture("Assets/Textures/skybox/bottom.jpg");
	Texture* textureFront = AssetManager::LoadTexture("Assets/Textures/skybox/front.jpg");
	Texture* textureBack = AssetManager::LoadTexture("Assets/Textures/skybox/back.jpg");
	defaultSkybox->ChangeTexture(TypeTexture::right, textureRight);
	defaultSkybox->ChangeTexture(TypeTexture::left, textureLeft);
	defaultSkybox->ChangeTexture(TypeTexture::top, textureTop);
	defaultSkybox->ChangeTexture(TypeTexture::bottom, texturebottom);
	defaultSkybox->ChangeTexture(TypeTexture::front, textureFront);
	defaultSkybox->ChangeTexture(TypeTexture::back, textureBack);
	SkyBox = std::make_unique<Skybox>(defaultSkybox);
}
Scene::~Scene() = default;