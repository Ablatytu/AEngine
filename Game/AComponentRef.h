#pragma once

#include "../GameAPI/GameABI.h"
#include "ComponentStorage.h"
#include "EngineAPIContext.h"
#include "EngineComponentWrapper.h"

template<typename T, bool IsEngine> struct AComponentRefStorage;

template<typename T> 
struct AComponentRefStorage<T, false>
{
	AE_COMPONENT_HANDLE handle{};
};


template<typename T>
struct AComponentRefStorage<T, true>
{
	AE_COMPONENT_HANDLE handle{};
	T componentWrapper{};
};

template<typename type>
class AComponentRef
{
public:
	AComponentRef() = default;

	explicit operator bool() const
	{
		if constexpr (EngineComponentWrapper<type>::supported)
		{

			const AE_API* api = ENGINEAPICONTEXT::Get();

			if (!api || !api->isEngineComponent) return false;

			return api->isEngineComponent(storage.handle);
		}
		else
		{
			static_assert(std::is_base_of_v<AComponent, type>, "THIS TYPE IS NOT SUPPORTED BY ACOMPONENTREF");
			return ComponentStorage::Get().ComponentIs<type>(storage.handle) != nullptr;
		}
	}

	type* operator->()
	{
		if constexpr (EngineComponentWrapper<type>::supported)
		{
			return &storage.componentWrapper;
		}
		else
		{
			return ComponentStorage::Get().ComponentIs<type>(storage.handle);
		}
	}
	
	type& operator*()
	{
		if constexpr (EngineComponentWrapper<type>::supported)
		{
			return storage.componentWrapper;
		}
		else
		{
			return *ComponentStorage::Get().ComponentIs<type>(storage.handle);
		}
	}
private:
	friend class GameObject;
	friend class ComponentStorage;

	explicit AComponentRef(AE_COMPONENT_HANDLE newHandle, AE_GAMEOBJECT_HANDLE newObjectHandle = {})
	{
		storage.handle = newHandle;
		if constexpr (EngineComponentWrapper<type>::supported)
		{
			storage.componentWrapper.ae_component_handle = newHandle;
			storage.componentWrapper.owner = newObjectHandle;
		}
	}
	AComponentRefStorage<type, EngineComponentWrapper<type>::supported> storage;
};