#pragma once
#include "../GameAPI/GameABI.h"

#include "AComponentRef.h"
#include "EngineComponentWrapper.h"
#include "EngineAPIContext.h"
#include "ComponentStorage.h"
#include <vector>
#include <memory>
class GameObject
{
public:
	GameObject() = default;

	explicit GameObject(AE_GAMEOBJECT_HANDLE newHandle) : handle(newHandle)
	{
	}

	explicit operator bool() const
	{
		return !AE_IsNull(handle);
	}

	AE_GAMEOBJECT_HANDLE GetHandle() const
	{
		return handle;
	}

	void Destroy();

	template<typename t> AComponentRef<t> GetAComponent() const
	{
		if constexpr (EngineComponentWrapper<t>::supported)
		{
			const AE_API* ae_api = ENGINEAPICONTEXT::Get();

			if (!ae_api)
				return {};

			if (!ae_api->getComponent)
				return {};

			AE_COMPONENT_HANDLE componentHandle{};

			const bool found = ae_api->getComponent(handle, &componentHandle, EngineComponentWrapper<t>::type);

			if (!found)
				return {};

			return AComponentRef<t>(componentHandle);
		}
		else
		{
			static_assert(std::is_base_of_v<AComponent, t>, "IS NOT ACOMPONENT");
			return AComponentRef<t>(ComponentStorage::Get().FindComponentHandle<t>(handle));
		}
	}

	template<typename t, typename... Args> AComponentRef<t> AddAComponent(Args&&... args) const
	{
		if constexpr (EngineComponentWrapper<t>::supported)
		{
			const AE_API* ae_api = ENGINEAPICONTEXT::Get();

			if (!ae_api)
				return {};

			if (!ae_api->addComponent)
				return {};

			AE_COMPONENT_HANDLE componentHandle;

			const bool found = ae_api->addComponent(handle, &componentHandle, EngineComponentWrapper<t>::type);

			if (!found)
				return {};
			return AComponentRef<t>(componentHandle);
		}
		else
		{
			static_assert(std::is_base_of_v<AComponent, t>, "THIS TYPE IS NOT SUPPORTED");

			return AComponentRef<t>(ComponentStorage::Get().Add<t>(handle, std::forward<Args>(args)...));
		}
	}

	static GameObject Create(const char* name = nullptr)
	{
		const AE_API* ae_api = ENGINEAPICONTEXT::Get();
		if (!ae_api)
			return {};

		AE_GAMEOBJECT_HANDLE ae_gameobject_handle{};
		if (!ae_api->createGameObject(name, &ae_gameobject_handle))
			return {};

		return GameObject(ae_gameobject_handle);
	}
private:
	AE_GAMEOBJECT_HANDLE handle{};
};