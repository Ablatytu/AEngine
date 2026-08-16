#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../GameAPI/GameABI.h"

class AComponent;

class ComponentStorage
{
public:
	struct ComponentSlot
	{
		std::unique_ptr<AComponent> component;
		uint32_t generation = 1;
	};

	static ComponentStorage& Get();                                                                                                                                                                                                                                                                                                                                                                                  

	bool CheckGameObjectHandle(AE_GAMEOBJECT_HANDLE handle, ComponentSlot& slot);
	template<typename T> AE_COMPONENT_HANDLE FindComponentHandle(AE_GAMEOBJECT_HANDLE handle)
	{
		for (int i = 0; i < slots.size(); i++)
		{
			ComponentSlot& slot = slots[i];

			if (!slot.component)
				continue;
			if (CheckGameObjectHandle(handle, slot))
				continue;

			auto casted = dynamic_cast<T*>(slot.component.get());
			if (casted)
				return AE_COMPONENT_HANDLE{ i, slot.generation };
		}

		return {};
	}
	template<typename T> T* ComponentIs(AE_COMPONENT_HANDLE handle)
	{
		if (handle.index >= slots.size()) return nullptr;

		ComponentSlot& slot = slots[handle.index];

		if (slot.generation != handle.generation) return nullptr;

		if (!slot.component) return nullptr;

		return static_cast<T*>(slot.component.get());
	}
	template<typename T, typename... Args> AE_COMPONENT_HANDLE Add(AE_GAMEOBJECT_HANDLE owner, Args&&... args)
	{
		uint32_t index = UINT32_MAX;

		for (uint32_t i = 0; i < slots.size(); ++i)
		{
			if (!slots[i].component)
			{
				index = i;
				break;
			}
		}

		if (index == UINT32_MAX)
		{
			slots.emplace_back();
			index = static_cast<uint32_t>(slots.size() - 1);
		}

		std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);

		AE_COMPONENT_HANDLE componentHandle{ index, slots[index].generation};

		component->owner = owner;
		component->ae_component_handle = componentHandle;

		startComponents.push_back(componentHandle);

		component->OnInspector();
		slots[index].component = std::move(component);
		return componentHandle;
	}

	void Remove(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE ae_component_handle);
	void RemoveAllGameComponents();
	void UpdateComponents(float deltaTime);
	void StartComponents();
	AE_COMPONENT_HANDLE ReturnComponentHandle(AComponent* component);
	int ReturnComponentCountFromGameObject(AE_GAMEOBJECT_HANDLE gameObject_handle);
	void ReturnComponentInfoFromGameObject(AE_GAMEOBJECT_HANDLE gameObject_handle, AE_COMPONENT_INFO* ae_component_handle, int size);
	void ReturnAllAComponentsInfo(AE_COMPONENT_INFO* ae_component_info_storage, int size);
	void RestoreAllAComponentsInfoFromInfo(AE_COMPONENT_INFO* ae_component_info_storage, int size);
	int GetAComponentCount();
private:
	std::vector<ComponentSlot> slots;
	
	std::vector<AE_COMPONENT_HANDLE> startComponents; 
};