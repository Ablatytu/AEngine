#include "GameObject.h"
#include "../components/Transform.h"
#include "../components/Camera.h"
#include "../components/Render.h"
#include "../scene/SceneManager.h"
#include "../scene/Scene.h"
#include "../components/ComponentStorage.h"
#include "../components/ComponentHandle.h"
#include "../Renderer/GameObjectStorage.h"
GameObject::GameObject()
{
	transform = AddComponent<Transform>();
	handle = GameObjectStorage::Get().Register(this);
}

GameObject::~GameObject()
{
	GameObjectStorage::Get().UnRegister(handle);
}
AEngineComponent* GameObject::AttachComponent(std::unique_ptr<AEngineComponent> component)
{
	component->gameObject = this;
	if (auto* camera = dynamic_cast<Camera*>(component.get()))
		SceneManager::Get().GetActiveScene().activeCamera = camera;
	if (auto* render = dynamic_cast<Render*>(component.get()))
		SceneManager::Get().GetActiveScene().renders.push_back(render);
	SceneManager::Get().GetActiveScene().AwakeAComponents.push_back(component.get());
	SceneManager::Get().GetActiveScene().StartAComponents.push_back(component.get());
	SceneManager::Get().GetActiveScene().AComponents.push_back(component.get());
	AEngineComponent* ptr = component.get();
	ptr->handle = ComponentStorage::Get().Register(ptr);
	components.push_back(std::move(component));
	return ptr;
}
void GameObject::RemoveComponent(AEngineComponent* component)
{
	for (int i = 0; i < components.size();)
	{
		if (component == components[i].get())
		{
			if (dynamic_cast<Transform*>(component))
				return;
			component->OnDestroy();
			ComponentStorage::Get().UnRegister(component->handle);
			if (auto d = dynamic_cast<Camera*>(component))
				SceneManager::Get().GetActiveScene().activeCamera = nullptr;
			if (auto d = dynamic_cast<Render*>(component))
				ClearComponentsVector(SceneManager::Get().GetActiveScene().renders, d);
			ClearComponentsVector(SceneManager::Get().GetActiveScene().AComponents, component);
			components.erase(components.begin() + i);
			ClearComponentsVector(SceneManager::Get().GetActiveScene().AwakeAComponents, component);
			ClearComponentsVector(SceneManager::Get().GetActiveScene().StartAComponents, component);
			break;
		}
		else
		{
			i++;
		}
	}
}

GameObjectHandle GameObject::getHandle() const
{
	return handle;
}