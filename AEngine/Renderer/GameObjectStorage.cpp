#include "GameObjectStorage.h"
#include "GameObject.h"

GameObjectStorage& GameObjectStorage::Get()
{
	static GameObjectStorage gameObjectStorage;
	return gameObjectStorage;
}

GameObjectHandle GameObjectStorage::Register(GameObject* gameObject)
{
	if (!gameObject)
		return {};
	for (uint32_t i = 0; i < slots.size(); i++)
	{
		Slot& slot = slots[i];

		if (!slot.ptr)
		{
			slot.ptr = gameObject;
			return GameObjectHandle{ i, slot.generation };
		}
	}

	Slot newSlot;

	newSlot.ptr = gameObject;

	slots.push_back(newSlot);

	return GameObjectHandle{ static_cast<uint32_t>(slots.size() - 1), newSlot.generation};
}

void GameObjectStorage::UnRegister(GameObjectHandle handle)
{
	if (handle.index >= slots.size())
		return;
	
	Slot& slot = slots[handle.index];

	if (slot.generation != handle.generation)
		return;

	slot.generation++;
	slot.ptr = nullptr;
}

GameObject* GameObjectStorage::Get(GameObjectHandle handle)
{
	if (handle.index >= slots.size())
		return nullptr;

	Slot& slot = slots[handle.index];

	if (slot.generation != handle.generation)
		return nullptr;
	return slot.ptr;
}