#include "EngineEntry.h"
#include "../Renderer/GameObject.h"
#include "../Renderer/GameObjectStorage.h"
#include "../scene/SceneManager.h"
#include "../scene/Scene.h"
#include "../scene/AssetManager.h"
#include "../Renderer/Texture.h"
#include "../core/Window.h"
#include "../Renderer/Material.h"
#include "../Renderer/RenderSystem.h"
#include "ComponentStorage.h"
#include "Transform.h"
#include "Render.h"
#include "Light.h"
#include "Camera.h"
#include "SpriteRenderer2D.h"
#include "ParticleSystem.h"
#include "AEngineComponent.h"
#include <iostream>

namespace
{
	void AE_CALL EngineLog(const char* logmessage)
	{
		std::cout << "[GAME.DLL] : " << logmessage << "\n";
	}

	bool AE_CALL EngineGetComponent(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE* ae_component_handle, AE_COMPONENT_TYPE ae_component_type)
	{
		if (!ae_component_handle)
			return false;

		*ae_component_handle = {};

		GameObject* findedGameobject = GameObjectStorage::Get().Get(ae_gameobject_handle);

		if (!findedGameobject)
			return false;
		AEngineComponent* ae_component = nullptr;
		switch (ae_component_type)
		{
			case AE_COMPONENT_TYPE::AE_TRANSFORM:
			{
				ae_component = findedGameobject->transform;
				break;
			}
			case AE_COMPONENT_TYPE::AE_CAMERA:
			{
				ae_component = findedGameobject->GetComponent<Camera>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_LIGHT:
			{
				ae_component = findedGameobject->GetComponent<Light>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_RENDERER:
			{
				ae_component = findedGameobject->GetComponent<Render>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_SPRITERENDERER2D:
			{
				ae_component = findedGameobject->GetComponent<SpriteRenderer2D>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_PARTICLESYSTEM:
			{
				ae_component = findedGameobject->GetComponent<ParticleSystem>();
				break;
			}
		}
		if (!ae_component)
			return false;

		*ae_component_handle = ae_component->handle;
		return true;

	}
	bool AE_CALL EngineAddComponent(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE* ae_component_handle, AE_COMPONENT_TYPE ae_component_type)
	{
		if (!ae_component_handle) return false;

		GameObject* findedObject = GameObjectStorage::Get().Get(ae_gameobject_handle);

		if (!findedObject) return false;

		AEngineComponent* engineComponent = nullptr;
		switch (ae_component_type)
		{
			case AE_COMPONENT_TYPE::AE_TRANSFORM:
			{
				engineComponent = findedObject->AddComponent<Transform>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_CAMERA:
			{
				engineComponent = findedObject->AddComponent<Camera>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_LIGHT:
			{
				engineComponent = findedObject->AddComponent<Light>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_RENDERER:
			{
				engineComponent = findedObject->AddComponent<Render>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_SPRITERENDERER2D:
			{
				engineComponent = findedObject->AddComponent<SpriteRenderer2D>();
				break;
			}
			case AE_COMPONENT_TYPE::AE_PARTICLESYSTEM:
			{
				engineComponent = findedObject->AddComponent<ParticleSystem>();
				break;
			}
			default:
			{
				return false;
				break;
			}
		}
		*ae_component_handle = engineComponent->handle;
		return true;
	}
	bool AE_CALL EngineGetTransformRotation(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3)
	{
		if (!ae_vec3)
			return false;

		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);

		if (!findedComponent)
			return false;

		Transform* findedTransform = dynamic_cast<Transform*>(findedComponent);

		if (!findedComponent)
			return false;

		glm::vec3 rot = glm::degrees(glm::eulerAngles(findedTransform->rotation));

		ae_vec3->x = rot.x;
		ae_vec3->y = rot.y;
		ae_vec3->z = rot.z;
		return true;
	}
	bool AE_CALL EngineSetTransformRotation(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3)
	{
		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);
		if (!findedComponent)
			return false;

		Transform* findedTransform = dynamic_cast<Transform*>(findedComponent);
		if (!findedComponent)
			return false;

		findedTransform->rotation = Quaternion::Euler(ae_vec3.x, ae_vec3.y, ae_vec3.z);
		findedTransform->UpdateMatrices();

		return true;
	}

	bool AE_CALL EngineIsEngineComponent(AE_COMPONENT_HANDLE ae_component_handle)
	{
		return ComponentStorage::Get().Get(ae_component_handle) != nullptr;
	}

	bool AE_CALL EngineGetTransformPosition(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3)
	{
		if (!ae_vec3)
			return false;

		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);

		if (!findedComponent)
			return false;

		Transform* transform = dynamic_cast<Transform*>(findedComponent);

		if (!transform)
			return false;

		ae_vec3->x = transform->position.x;
		ae_vec3->y = transform->position.y;
		ae_vec3->z = transform->position.z;

		return true;
	}

	bool AE_CALL EngineGetTransformScale(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3)
	{
		if (!ae_vec3)
			return false;

		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);

		if (!findedComponent)
			return false;

		Transform* transform = dynamic_cast<Transform*>(findedComponent);

		if (!transform)
			return false;

		ae_vec3->x = transform->scale.x;
		ae_vec3->y = transform->scale.y;
		ae_vec3->z = transform->scale.z;

		return true;
	}
	void AE_CALL EngineGetTransformForward(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3)
	{
		if (!ae_vec3)
			return;

		AEngineComponent* FindedComponent = ComponentStorage::Get().Get(ae_transform_component);
		if (!FindedComponent)
			return;

		Transform* FindedTransform = dynamic_cast<Transform*>(FindedComponent);
		if (!FindedTransform)
			return;

		glm::vec3 ae_forward = FindedTransform->forward();

		ae_vec3->x = ae_forward.x;
		ae_vec3->y = ae_forward.y;
		ae_vec3->z = ae_forward.z;
	}
	void AE_CALL EngineGetTransformRight(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3)
	{
		if (!ae_vec3)
			return;

		AEngineComponent* FindedComponent = ComponentStorage::Get().Get(ae_transform_component);
		if (!FindedComponent)
			return;

		Transform* FindedTransform = dynamic_cast<Transform*>(FindedComponent);
		if (!FindedTransform)
			return;

		glm::vec3 ae_right = FindedTransform->right();

		ae_vec3->x = ae_right.x;
		ae_vec3->y = ae_right.y;
		ae_vec3->z = ae_right.z;
	}
	bool AE_CALL EngineSetTransformScale(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3)
	{
		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);

		if (!findedComponent)
			return false;

		Transform* transform = dynamic_cast<Transform*>(findedComponent);

		if (!transform)
			return false;

		transform->scale.x = ae_vec3.x;
		transform->scale.y = ae_vec3.y;
		transform->scale.z = ae_vec3.z;

		transform->UpdateMatrices();

		return true;

	}

	bool AE_CALL EngineSetTransfromRotate(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3)
	{
		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);
		if (!findedComponent)
			return false;

		Transform* findedTransform = dynamic_cast<Transform*>(findedComponent);
		if (!findedTransform)
			return false;

		glm::quat rot = Quaternion::Euler(ae_vec3.x, ae_vec3.y, ae_vec3.z);

		findedTransform->rotation = glm::normalize(findedTransform->rotation * rot);

		findedTransform->UpdateMatrices();

		return true;
	}

	bool AE_CALL EngineSetTransformPosition(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3)
	{
		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);

		if (!findedComponent)
			return false;

		Transform* transform = dynamic_cast<Transform*>(findedComponent);

		if (!transform)
			return false;

		transform->position.x = ae_vec3.x;
		transform->position.y = ae_vec3.y;
		transform->position.z = ae_vec3.z;

		transform->UpdateMatrices();

		return true;

	}

	void AE_CALL EngineSetCameraProjection(AE_COMPONENT_HANDLE ae_camera_handle, uint8_t ae_type_projection)
	{
		if (ae_type_projection > 1)
			return;

		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_camera_handle);
		if (!findedComponent)
			return;

		Camera* findedCamera = dynamic_cast<Camera*>(findedComponent);
		if (!findedCamera)
			return;

		findedCamera->projectionType = ae_type_projection == 0 ? Camera::ProjectionType::Perspective : Camera::ProjectionType::Orthographic;
	}
	bool AE_CALL EngineCreateGameObject(const char* name, AE_GAMEOBJECT_HANDLE* ae_gameobject_handle)
	{
		if (!ae_gameobject_handle)
			return false;
		GameObject* newObject = SceneManager::Get().CreateGameObject();
		if (!newObject)
			return false;
		if (name)
			newObject->name = name;
		*ae_gameobject_handle = newObject->getHandle();
		return true;
	}
	void AE_CALL EngineDestroyGameObject(AE_GAMEOBJECT_HANDLE ae_gameobject_handle)
	{
		GameObject* destroyObject = GameObjectStorage::Get().Get(ae_gameobject_handle);
		if (!destroyObject)
			return;

		destroyObject->isDestroying = true;
		SceneManager::Get().GetActiveScene().SetDestroyingObject(ae_gameobject_handle);
	}
	bool AE_CALL EngineIsGameObject(AE_GAMEOBJECT_HANDLE ae_gameobject_handle)
	{
		return GameObjectStorage::Get().Get(ae_gameobject_handle) != nullptr;
	}
	bool AE_CALL EngineGetTexture(const char* name, AE_GAME_RESOURCES_INFO::AE_TEXTURE* texture)
	{
		Texture* loadedTexture = AssetManager::LoadTexture(name);
		if (loadedTexture && texture)
		{
			texture->name = name;
			return true;
		}
		return false;
	}
	bool AE_CALL EngineGetShader(const char* vertexPath, const char* fragmentPath, AE_GAME_RESOURCES_INFO::AE_SHADER* infoShader)
	{
		if (!vertexPath || !fragmentPath || !infoShader)
			return false;

		Shader* FindedShader = AssetManager::LoadShader(vertexPath, fragmentPath);

		if (FindedShader)
		{
			infoShader->AE_VERTEX_SHADER_PATH = vertexPath;
			infoShader->AE_FRAGMENT_SHADER_PATH = fragmentPath;
			return true;
		}
		return false;
	}
	bool AE_CALL EngineGetMaterial(const char* name, AE_GAME_RESOURCES_INFO::AE_MATERIAL* ae_material)
	{
		if (!name || !ae_material)
			return false;

		Material* FindedMaterial = AssetManager::CreateMaterial(name, nullptr);

		if (!FindedMaterial)
			return false;

		ae_material->name = name;
		return true;
	}
	bool AE_CALL EngineGetRenderImage(const char* name, AE_GAME_RESOURCES_INFO::AE_RENDERIMAGE* ae_renderimage, AE_COMPONENT_HANDLE ae_camera_handle)
	{
		if (!name || !ae_renderimage)
			return false;

		AEngineComponent* FindedComponent = ComponentStorage::Get().Get(ae_camera_handle);
		if (!FindedComponent)
			return false;

		Camera* FindedCamera = dynamic_cast<Camera*>(FindedComponent);
		if (!FindedCamera)
			return false;

		if (FindedCamera->GetFramebuffer(name))
		{
			ae_renderimage->name = name;
			return true;
		}

		return false;
	}
	bool AE_CALL EngineRenderFrameBufferCustomShader(AE_GAME_RESOURCES_INFO::AE_RENDERIMAGE ae_renderimage_source, AE_GAME_RESOURCES_INFO::AE_RENDERIMAGE ae_renderimage_destination, AE_COMPONENT_HANDLE ae_camera_hadle, AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material)
	{
		Camera* FindedCameraComponent = static_cast<Camera*>(ComponentStorage::Get().Get(ae_camera_hadle));
		Framebuffer* source = FindedCameraComponent->GetFramebuffer(ae_renderimage_source.name);
		Framebuffer* output = FindedCameraComponent->GetFramebuffer(ae_renderimage_destination.name);
		if (!ae_material.name)
		{
			RenderSystem::rs_AddGameCommand({ nullptr, source, output });
			return true;
		}
		Material* mat = AssetManager::LoadMaterial(ae_material.name);
		if (source && output && mat)
		{
			RenderSystem::rs_AddGameCommand({ mat, source, output });
			return true;
		}
		return false;
	}
	bool AE_CALL EngineGetParticleSystemMaterial(AE_COMPONENT_HANDLE ae_component_handle, AE_GAME_RESOURCES_INFO::AE_MATERIAL* ae_material)
	{
		if (!ae_material)
			return false;

		AEngineComponent* FindedAComponent = ComponentStorage::Get().Get(ae_component_handle);
		if (!FindedAComponent)
			return false;

		ParticleSystem* FindedParticleSystem = dynamic_cast<ParticleSystem*>(FindedAComponent);
		if (!FindedParticleSystem)
			return false;

		ae_material->name = FindedParticleSystem->material->path.c_str();
		return true;
	}
	bool AE_CALL EngineSetParticleSystemMaterial(AE_COMPONENT_HANDLE ae_component_handle, AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material)
	{
		AEngineComponent* FindedAComponent = ComponentStorage::Get().Get(ae_component_handle);
		if (!FindedAComponent)
			return false;

		ParticleSystem* FindedParticleSystem = dynamic_cast<ParticleSystem*>(FindedAComponent);
		if (!FindedParticleSystem)
			return false;

		FindedParticleSystem->material = AssetManager::LoadMaterial(ae_material.name);
		return true;
	}
	bool AE_CALL EngineSetMaterialTexture(AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material, const char* nameField, AE_GAME_RESOURCES_INFO::AE_TEXTURE ae_texture)
	{
		Material* FindedMaterial = AssetManager::LoadMaterial(ae_material.name);
		if (!FindedMaterial)
			return false;
		Texture* FindedTexture = AssetManager::LoadTexture(ae_texture.name);

		FindedMaterial->Set(nameField, FindedTexture);
		return true;
	}
	void AE_CALL EngineSetMateterialValue(const char* ae_name_field_of_material, AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material, const void* ae_value, int ae_type_material_field)
	{
		if (!ae_name_field_of_material || !ae_value)
			return;

		Material* FindedMaterial = AssetManager::LoadMaterial(ae_material.name);
		if (!FindedMaterial)
			return;

		switch (ae_type_material_field)
		{
		case 0:
		{
			FindedMaterial->Set(ae_name_field_of_material, *static_cast<const int*>(ae_value));
			break;
		}
		case 1:
		{
			FindedMaterial->Set(ae_name_field_of_material, *static_cast<const bool*>(ae_value));
			break;
		}
		default:
			break;
		}
	}
	bool AE_CALL EngineSetSpriteTexture(const char* name, AE_COMPONENT_HANDLE ae_component_handle)
	{
		if (!name)
			return false;

		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_component_handle);
		if (!findedComponent)
			return false;

		SpriteRenderer2D* findedSpriteRenderer2D = dynamic_cast<SpriteRenderer2D*>(findedComponent);
		if (!findedSpriteRenderer2D)
			return false;

		findedSpriteRenderer2D->texture = AssetManager::LoadTexture(name);
		return true;
	}
	bool AE_CALL EngineSetSpriteColor(AE_VEC4 color, AE_COMPONENT_HANDLE ae_component_handle)
	{
		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_component_handle);
		if (!findedComponent)
			return false;

		SpriteRenderer2D* findedSpriteRenderer2D = dynamic_cast<SpriteRenderer2D*>(findedComponent);
		if (!findedSpriteRenderer2D)
			return false;

		findedSpriteRenderer2D->color = glm::vec3(color.x, color.y, color.z);
		return true;
	}
	bool AE_CALL EngineGetKey(int32_t key)
	{
		return Input::GetKey(key);
	}
	float AE_CALL EngineGetAxis(const char* AE_AXIS_NAME)
	{
		return Input::GetAxis(AE_AXIS_NAME);
	}
}

namespace EngineAPI
{
	void BuildAPI(AE_API& ae_api)
	{
		ae_api = {};

		ae_api.log = &EngineLog;
		ae_api.getComponent = &EngineGetComponent;

		ae_api.getTransformPosition = &EngineGetTransformPosition;
		ae_api.getTransformRotation = &EngineGetTransformRotation;
		ae_api.getTransformScale = &EngineGetTransformScale;
		ae_api.getTransformForward = &EngineGetTransformForward;
		ae_api.getTransformRight = &EngineGetTransformRight;

		ae_api.setTransformRotation = &EngineSetTransformRotation;
		ae_api.setTransformPosition = &EngineSetTransformPosition;
		ae_api.setTransformScale = &EngineSetTransformScale;

		ae_api.setTransformRotate = &EngineSetTransfromRotate;
		ae_api.addComponent = &EngineAddComponent;
		ae_api.isEngineComponent = &EngineIsEngineComponent;
		ae_api.createGameObject = &EngineCreateGameObject;
		ae_api.getTexture = &EngineGetTexture;
		ae_api.setSpriteTexture = &EngineSetSpriteTexture;
		ae_api.setSpriteColor = &EngineSetSpriteColor;
		ae_api.setParticleSystemMaterial = &EngineSetParticleSystemMaterial;
		ae_api.getParticleSystemMaterial = &EngineGetParticleSystemMaterial;
		ae_api.setMaterialTexture = &EngineSetMaterialTexture;
		ae_api.setMaterialValueField = &EngineSetMateterialValue;
		ae_api.getKey = &EngineGetKey;
		ae_api.getAxis = &EngineGetAxis;
		ae_api.destroyGameObject = &EngineDestroyGameObject;
		ae_api.isGameObject = &EngineIsGameObject;
		ae_api.getShader = &EngineGetShader;
		ae_api.getMaterial = &EngineGetMaterial;
		ae_api.getRenderImage = &EngineGetRenderImage;
		ae_api.blitRenderFrame = &EngineRenderFrameBufferCustomShader;
		ae_api.setCameraProjection = &EngineSetCameraProjection;
	}
}