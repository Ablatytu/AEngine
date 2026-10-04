#include "ComponentStorage.h"
#include "AEngineComponent.h"
#include "../Renderer/GameObject.h"

ComponentStorage& ComponentStorage::Get()
{
	static ComponentStorage storage;
	return storage;
}

ComponentHandle ComponentStorage::Register(AEngineComponent* component)
{
	for (uint32_t i = 0; i < slots.size(); i++)
	{
		if (slots[i].ptr == nullptr)
		{
			slots[i].ptr = component;
			return ComponentHandle{ i, slots[i].generation};
		}
	}
	Slot newSlot;
	newSlot.ptr = component;
	slots.push_back(newSlot);

	uint32_t index = static_cast<uint32_t>(slots.size() - 1);
	return ComponentHandle{ index, newSlot.generation };
}

void ComponentStorage::UnRegister(ComponentHandle handle)
{
	if (handle.index >= slots.size())
		return;

	Slot& slot = slots[handle.index];

	if (slot.generation != handle.generation)
		return;

	slot.ptr = nullptr;
	slot.generation++;
}

AEngineComponent* ComponentStorage::Get(ComponentHandle handle)
{
	if (handle.index >= slots.size())
		return nullptr;

	const Slot& slot = slots[handle.index];

	if (slot.generation != handle.generation)
		return nullptr;
		
	if (!slot.ptr)
		return nullptr;

	if (!slot.ptr->gameObject)
		return nullptr;

	if (slot.ptr->gameObject->isDestroying)
		return nullptr;

	return slot.ptr;
}