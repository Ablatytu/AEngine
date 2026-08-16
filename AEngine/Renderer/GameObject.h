#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <vector>
#include "../components/AEngineComponent.h"
#include "GameObjectHandle.h"
#include <memory>
#include <algorithm>
#include <string>
#include "../core/AEngineAPI.h"
#include <iostream>

class Transform;

class GameObject
{
public:
	std::string name = "New GameObject";
	std::vector<std::unique_ptr<AEngineComponent>> components;
	Transform* transform;
	template<typename T, typename... Args> T* AddComponent(Args&&... args)
	{
		auto component = std::make_unique<T>(std::forward<Args>(args)...);
		T* result = component.get();
		AttachComponent(std::move(component));
		return result;
	}
	AENGINE_API AEngineComponent* AttachComponent(std::unique_ptr<AEngineComponent> component);
	template<typename T> T* GetComponent()
	{
		for (auto& _component : components)
		{
			auto casted = dynamic_cast<T*>(_component.get());
			if (casted)
				return casted;
		}
		return nullptr;
	}

	template<typename T> void ClearComponentsVector(std::vector<T*>& components, T* component)
	{
		components.erase(std::remove(components.begin(), components.end(), component), components.end());
	}

	AENGINE_API void RemoveComponent(AEngineComponent* component);

	AENGINE_API GameObject();
	AENGINE_API ~GameObject();
	AENGINE_API GameObjectHandle getHandle() const;

private:
	GameObjectHandle handle{};
};
#endif