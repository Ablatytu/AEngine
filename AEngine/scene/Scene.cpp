#include "Scene.h"
#include <memory>
#include "../Renderer/GameObject.h"
#include "Skybox.h"
#include "../Renderer/Cubemap.h"
#include "AssetManager.h"
#include "../Renderer/RenderContext.h"
#include "../Renderer/RenderSystem.h"
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
	context.skybox = skyBox.get();
	context.lights = A_LightManager->GetLights();
	context.renders = renders;
	context.HDR = HDR;
	context.postProcess = postProcess;
	context.irradianceMap = irradianceMap.get();
	context.prefilterMap = preilterCubemap.get();
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
	Cubemap* defaultSkybox = AssetManager::CreateCubemap("defaultSkyBox", 2048);
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
	SetSkyBox(defaultSkybox);
}
void Scene::SetSkyBox(Cubemap* cubemap)
{
	cubemap->CreateMipmap();
	if (!skyBox)
		skyBox = std::make_unique<Skybox>(cubemap);
	else
		skyBox->ChangeSkyBox(cubemap);
	std::unique_ptr<Cubemap> irradianceCubeMap = std::make_unique<Cubemap>(32, true);
	RenderSystem::rs_RenderIrradianceMap(*cubemap, *irradianceCubeMap, 32, 32);
	std::unique_ptr<Cubemap> prefiltercubemap = std::make_unique<Cubemap>(128, true, true);
	RenderSystem::rs_RenderPrefilterMap(*cubemap, *prefiltercubemap);
	irradianceMap = std::move(irradianceCubeMap);
	preilterCubemap = std::move(prefiltercubemap);
}

void Scene::SetSkyBox(HDRTexture* texture)
{
	environmentcubemap = std::make_unique<Cubemap>(512, true);
	RenderSystem::rs_RenderEnvironmentHDR(*environmentcubemap, *texture, 512, 512);
	SetSkyBox(environmentcubemap.get());
}
Scene::~Scene() = default;