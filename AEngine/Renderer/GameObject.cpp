#include "GameObject.h"
#include "../components/Transform.h"
#include "../components/Camera.h"
#include "../components/Render.h"
#include "../components/SpriteRenderer2D.h"
#include "../components/ParticleSystem.h"
#include "../scene/SceneManager.h"
#include "../scene/Scene.h"
#include "../components/ComponentStorage.h"
#include "../components/ComponentHandle.h"
#include "../Renderer/GameObjectStorage.h"
GameObject::GameObject()
{
	handle = GameObjectStorage::Get().Register(this);
	transform = AddComponent<Transform>();
}
void GameObject::WillDestroy()
{
	while (!components.empty())
	{
		RemoveComponent(components.back().get());
	}
	components.clear();
	transform = nullptr;
}
GameObject::~GameObject()
{
	WillDestroy();
	GameObjectStorage::Get().UnRegister(handle);
}
AEngineComponent* GameObject::AttachComponent(std::unique_ptr<AEngineComponent> component)
{
	component->gameObject = this;
	AEngineComponent* ptr = component.get();
	ptr->handle = ComponentStorage::Get().Register(ptr);
	components.push_back(std::move(component));
	if (auto* camera = dynamic_cast<Camera*>(ptr))
		SceneManager::Get().GetActiveScene().RegisterCamera(camera);
	if (auto* render = dynamic_cast<Render*>(ptr))
		SceneManager::Get().GetActiveScene().renders.push_back(render);
	if (auto* sprite = dynamic_cast<SpriteRenderer2D*>(ptr))
		SceneManager::Get().GetActiveScene().sprites.push_back(sprite);
	if (auto* particleSystem = dynamic_cast<ParticleSystem*>(ptr))
		SceneManager::Get().GetActiveScene().ParticleSystems.push_back(particleSystem);
	SceneManager::Get().GetActiveScene().StartAComponents.push_back(ptr);
	SceneManager::Get().GetActiveScene().AComponents.push_back(ptr);
	ptr->Awake();
	return ptr;
}
void GameObject::RemoveComponent(AEngineComponent* component)
{
	for (int i = 0; i < components.size();)
	{
		if (component == components[i].get())
		{
			component->OnDestroy();
			ComponentStorage::Get().UnRegister(component->handle);
			if (auto d = dynamic_cast<Camera*>(component))
				SceneManager::Get().GetActiveScene().UnregisterCamera(d);
			if (auto d = dynamic_cast<Render*>(component))
				ClearComponentsVector(SceneManager::Get().GetActiveScene().renders, d);
			if (auto d = dynamic_cast<SpriteRenderer2D*>(component))
				ClearComponentsVector(SceneManager::Get().GetActiveScene().sprites, d);
			if (auto d = dynamic_cast<ParticleSystem*>(component))
				ClearComponentsVector(SceneManager::Get().GetActiveScene().ParticleSystems, d);
			ClearComponentsVector(SceneManager::Get().GetActiveScene().AComponents, component);
			ClearComponentsVector(SceneManager::Get().GetActiveScene().StartAComponents, component);
			components.erase(components.begin() + i);
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