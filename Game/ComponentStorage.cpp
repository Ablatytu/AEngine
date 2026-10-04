#include "ComponentStorage.h"
#include "AComponent.h"
#include "EngineAPIContext.h"

ComponentStorage& ComponentStorage::Get()
{
	static ComponentStorage storage;
	return storage;
}

AE_COMPONENT_HANDLE ComponentStorage::ReturnComponentHandle(AComponent* component)
{
	return component->GetHandle();
}

void ComponentStorage::UpdateComponents(float deltaTime)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;

	for (ComponentSlot& slot : slots)
	{
		if (!slot.component) continue;
		if (!ae_api->isGameObject(slot.component->owner)) continue;

		slot.component->Update(deltaTime);
	}
}
void ComponentStorage::StartComponents()
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;
	while (!startComponents.empty())
	{
		std::vector<AE_COMPONENT_HANDLE> currentComponentsHandles{};
		currentComponentsHandles.swap(startComponents);
		for (AE_COMPONENT_HANDLE& startslot : currentComponentsHandles)
		{
			if (startslot.index >= slots.size()) continue;
			ComponentSlot& slot = slots[startslot.index];
			if (startslot.generation != slot.generation) continue;
			if (!slot.component) continue;

			if(!ae_api->isGameObject(slot.component->owner)) continue;

 			slot.component->Start();
		}
	}
}

bool ComponentStorage::ComponentHasGameObject(AComponent* component)
{
	const AE_API* api = ENGINEAPICONTEXT::Get();
	if (!api)
		return false;

	if (!api->isGameObject(component->owner))
		return false;
	return true;
}
bool ComponentStorage::CheckGameObjectHandle(AE_GAMEOBJECT_HANDLE handle, ComponentSlot& slot)
{
	return (slot.component->owner.index != handle.index || slot.component->owner.generation != handle.generation);
}

void ComponentStorage::RemoveAllFromGameObject(AE_GAMEOBJECT_HANDLE ae_gameobject_handle)
{
	for (ComponentSlot& slot : slots)
	{
		if (!slot.component) continue;

		if (!CheckGameObjectHandle(ae_gameobject_handle, slot))
		{
			slot.generation++;
			slot.component.reset();
			slot.component = nullptr;
		}
	}
}

int ComponentStorage::ReturnComponentCountFromGameObject(AE_GAMEOBJECT_HANDLE gameObject_handle)
{
	int result = 0;
	for (int i = 0; i < slots.size(); i++)
	{
		ComponentSlot& slot = slots[i];

		if (!slot.component) continue;
		if (CheckGameObjectHandle(gameObject_handle, slot)) continue;

		result++;
	}
	return result;
}
void ComponentStorage::ReturnComponentInfoFromGameObject(AE_GAMEOBJECT_HANDLE gameObject_handle, AE_COMPONENT_INFO* ae_component_handle, int size)
{
	if (!ae_component_handle || size <= 0)
		return;
	int sizeVector = 0;
	for (int i = 0; i < slots.size(); i++)
	{
		if (sizeVector >= size) break;
		ComponentSlot& slot = slots[i];

		if (!slot.component) continue;
		
		if (CheckGameObjectHandle(gameObject_handle, slot)) continue;

		ae_component_handle[sizeVector++] = AE_COMPONENT_INFO{slot.component->owner, slot.component->GetHandle(), slot.component->GetComponentName(), slot.component->fields.data(), static_cast<int>(slot.component->fields.size())};
	}
}

void ComponentStorage::ReturnAllAComponentsInfo(AE_COMPONENT_INFO* ae_component_info_storage, int size)
{
	if (size > slots.size())
	{
		std::cout << "SIZE BIGGER THAТ SLOTS SIZE\n";
		return;
	}
	for (unsigned int i = 0; i < size; i++)
	{
		ComponentSlot& slot = slots[i];
		if (slot.component == nullptr) continue;

		ae_component_info_storage[i].name = slot.component->GetComponentName();
		ae_component_info_storage[i].countFieldsInfo = static_cast<int>(slot.component->fields.size());
		ae_component_info_storage[i].ae_gameObject_handle = slot.component->owner;
		ae_component_info_storage[i].ae_component_handle = { i, slot.generation };
		ae_component_info_storage[i].fieldsInfo = slot.component->fields.data();
	}
}

void ComponentStorage::Remove(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE ae_component_handle)
{
	if (ae_component_handle.index >= slots.size()) return;
	ComponentSlot& slot = slots[ae_component_handle.index];
	if (!slot.component) return;

	if (CheckGameObjectHandle(ae_gameobject_handle, slot)) return;

	if (slot.generation == ae_component_handle.generation)
	{
		slot.component->OnDestroy();

		slot.component.reset();
		slot.generation++;
	}
}
int ComponentStorage::GetAComponentCount()
{
	return slots.size();
}

void ComponentStorage::RemoveAllGameComponents()
{
	for (ComponentSlot& slot : slots)
	{
		if (!slot.component) continue;

		slot.component.reset();
		++slot.generation;
	}
	startComponents.clear();
}