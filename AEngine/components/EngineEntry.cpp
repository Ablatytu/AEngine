#include "EngineEntry.h"
#include "../Renderer/GameObject.h"
#include "../Renderer/GameObjectStorage.h"
#include "ComponentStorage.h"
#include "Transform.h"
#include "Render.h"
#include "Light.h"
#include "Camera.h"
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
			default:
				return false;
				break;
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

		Transform* findedTransform = static_cast<Transform*>(findedComponent);

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

		Transform* findedTransform = static_cast<Transform*>(findedComponent);
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

	bool AE_CALL EngineSetTransfromRotate(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3)
	{
		AEngineComponent* findedComponent = ComponentStorage::Get().Get(ae_transform_component);
		if (!findedComponent)
			return false;

		Transform* findedTransform = static_cast<Transform*>(findedComponent);
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
		ae_api.setTransformRotation = &EngineSetTransformRotation;
		ae_api.setTransformPosition = &EngineSetTransformPosition;
		ae_api.setTransformRotate = &EngineSetTransfromRotate;
		ae_api.addComponent = &EngineAddComponent;
		ae_api.isEngineComponent = &EngineIsEngineComponent;
	}
}