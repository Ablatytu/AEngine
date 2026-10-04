#pragma once
#include <vector>
#include <memory>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <string>
#include "../core/AEngineAPI.h"
#include "../../GameAPI/GameABI.h"

class GameObject;
class AEngineComponent;
class Render;
class Camera;
class Skybox;
class Light;
class RenderContext;
struct RenderContext2D;
class LightManager;
class PostProcess;
class Cubemap;
class HDRTexture;
class SpriteRenderer2D;
class ParticleSystem;

class Scene
{
	public:
		std::vector<std::unique_ptr<GameObject>> objects;
		std::vector<AEngineComponent*> AComponents;
		std::vector<AEngineComponent*> AwakeAComponents;
		std::vector<AEngineComponent*> StartAComponents;
		std::vector<Render*> renders;
		std::vector<SpriteRenderer2D*> sprites;
		std::vector<ParticleSystem*> ParticleSystems;
		std::vector<AE_GAMEOBJECT_HANDLE> destroyingObjects;
		PostProcess* postProcess = nullptr;
		struct FogSettings
		{
			bool Fog = false;
			glm::vec3 FogColor = glm::vec3(1.0f);
		};
		FogSettings fogSettings;
		bool HDR = true;
		Camera* activeCamera = nullptr;
		std::vector<Camera*> cameras;
		void RegisterCamera(Camera* newCamera);
		AENGINE_API void UnregisterCamera(Camera* oldCamera);
		AENGINE_API GameObject* CreateGameObject();
		AENGINE_API RenderContext CollectRenderContext(Camera* camera);
		AENGINE_API RenderContext CollectRenderContext();
		AENGINE_API RenderContext2D CollectRender2DContext();
		AENGINE_API RenderContext2D CollectRender2DContext(Camera* camera);
		void DestroyDestroyingObjects();
		AENGINE_API void EraseScene();
		void SetDestroyingObject(AE_GAMEOBJECT_HANDLE handle);
		std::unique_ptr<Skybox> skyBox;
		std::unique_ptr<LightManager> A_LightManager;
		std::unique_ptr<Cubemap> irradianceMap;
		std::unique_ptr<Cubemap> preilterCubemap;
		std::unique_ptr<Cubemap> environmentcubemap;
		AENGINE_API void SetSkyBox(Cubemap* cubemap);
		AENGINE_API void SetSkyBox(HDRTexture* texture);
		AENGINE_API Scene();
		AENGINE_API ~Scene();
		std::string pathScene = "";
};