#pragma once

#include <cstdint>
#include <vector>

#include "GameObjectHandle.h"

class GameObject;

class GameObjectStorage
{
public:
	struct Slot
	{
		GameObject* ptr = nullptr;
		uint32_t generation = 1;
	};

	static GameObjectStorage& Get();

	GameObjectHandle Register(GameObject* gameObject);
	void UnRegister(GameObjectHandle handle);

	GameObject* Get(GameObjectHandle handle);

private:
	std::vector<Slot> slots;
};