#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <vector>
#include "../components/AComponent.h"
#include "../scene/SceneManager.h"
#include "../components/Render.h"
#include "../components/Camera.h"
#include "../components/Light.h"
#include <memory>
#include "../components/Transform.h"
#include <algorithm>
#include <string>
#include "../scene/Scene.h"
class GameObject
{
public:
	std::string name = "New GameObject";
	std::vector<std::unique_ptr<AComponent>> components;
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

	template<typename T> T* AddComponent()
	{
		std::unique_ptr<T> _component = std::make_unique<T>();
		T* ptr = _component.get();
		_component->gameObject = this;
		if constexpr (std::is_base_of_v<Camera, T>)
			SceneManager::Get().GetActiveScene().activeCamera = _component.get();
		if constexpr (std::is_base_of_v<Render, T>)
			SceneManager::Get().GetActiveScene().renders.push_back(_component.get());
		SceneManager::Get().GetActiveScene().AwakeAComponents.push_back(_component.get());
		SceneManager::Get().GetActiveScene().StartAComponents.push_back(_component.get());
		SceneManager::Get().GetActiveScene().AComponents.push_back(_component.get());
		components.push_back(std::move(_component));
		return ptr;
	}
	template<typename T> void ClearComponentsVector(std::vector<T*>& components, T* component)
	{
		components.erase(std::remove(components.begin(), components.end(), component), components.end());
	}
	void RemoveComponent(AComponent* component)
	{
		for (int i = 0; i < components.size();)
		{
			if (component == components[i].get())
			{
				if (dynamic_cast<Transform*>(component))
					return;
				component->OnDestroy();
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
	GameObject()
	{
		AddComponent<Transform>();
	}
};
#endif