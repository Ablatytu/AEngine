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
#include "../components/SpriteRenderer2D.h"
#include "../Renderer/HDRTexture.h"
#include "../components/GameModule.h"
#include "../Renderer/RenderContext2D.h"
#include <unordered_set>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderContext Scene::CollectRenderContext(Camera* camera)
{
	RenderContext context;
	context.camera = camera;
	context.fog.enabled = fogSettings.Fog;
	context.fog.fogColor = fogSettings.FogColor;
	context.skybox = skyBox.get();
	context.lights = A_LightManager->GetLights();
	context.renders = renders;
	context.HDR = HDR;
	context.postProcess = postProcess;
	context.irradianceMap = irradianceMap.get();
	context.prefilterMap = preilterCubemap.get();
	context.ParticleSystems = ParticleSystems;
	for (SpriteRenderer2D* sprite : sprites)
	{
		if (sprite && sprite->renderSpace == SpriteRenderer2D::RenderSpace::World)
			context.sprites.push_back(sprite);
	}
	return context;
}
RenderContext2D Scene::CollectRender2DContext(Camera* camera)
{
	RenderContext2D context;
	context.camera = camera;
	for (SpriteRenderer2D* sprite : sprites)
	{
		if(sprite && sprite->renderSpace == SpriteRenderer2D::RenderSpace::Canvas)
			context.sprites.push_back(sprite);
	}
	return context;
}
RenderContext Scene::CollectRenderContext()
{
	return CollectRenderContext(activeCamera);
}
RenderContext2D Scene::CollectRender2DContext()
{
	return CollectRender2DContext(activeCamera);
}
void Scene::RegisterCamera(Camera* newCamera)
{
	cameras.push_back(newCamera);
	activeCamera = newCamera;
}
void Scene::UnregisterCamera(Camera* oldCamera)
{
	cameras.erase(std::remove(cameras.begin(), cameras.end(), oldCamera), cameras.end());
	if (oldCamera == activeCamera)
		activeCamera = cameras.empty() ? nullptr : cameras.back();
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
	HDRTexture* defaultSkybox = AssetManager::LoadHDRTexture("Assets/Textures/HDR/citrus_orchard_road_puresky_2k.hdr");
	SetSkyBox(defaultSkybox);
}
void Scene::EraseScene()
{
	AComponents.clear();
	StartAComponents.clear();
	GameModule::GETGAMECONTEXT().clearGameAComponents();
	renders.clear();
	sprites.clear();
	ParticleSystems.clear();
	objects.clear();
	postProcess = nullptr;
	activeCamera = nullptr;
	cameras.clear();
	A_LightManager->EraseLights();
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
void Scene::DestroyDestroyingObjects()
{
	while (!destroyingObjects.empty())
	{
		std::vector<AE_GAMEOBJECT_HANDLE> savedDestroyingObjects{};
		savedDestroyingObjects.swap(destroyingObjects);
		for (AE_GAMEOBJECT_HANDLE& destroy : savedDestroyingObjects)
		{
			auto it = std::find_if(objects.begin(), objects.end(), [&](const std::unique_ptr<GameObject>& finded) {return finded && finded->getHandle().generation == destroy.generation && finded->getHandle().index == destroy.index; });
			if (it != objects.end())
			{
				GameModule::GETGAMECONTEXT().removeAComponentsFromGameObject(destroy);
				objects.erase(it);
			}
		}
	}
}
void Scene::SetDestroyingObject(AE_GAMEOBJECT_HANDLE handle)
{
	destroyingObjects.push_back(handle);
}
void Scene::SetSkyBox(HDRTexture* texture)
{
	environmentcubemap = std::make_unique<Cubemap>(512, true);
	environmentcubemap->path = texture->path;
	RenderSystem::rs_RenderEnvironmentHDR(*environmentcubemap, *texture, 512, 512);
	SetSkyBox(environmentcubemap.get());
}
Scene::~Scene() = default;