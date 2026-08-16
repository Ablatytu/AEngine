#pragma once
#include <iostream>
#include <vector>
#include "../core/AEngineAPI.h"
#include "ComponentHandle.h"

class AEngineComponent;

class AENGINE_API ComponentStorage
{
public:
	struct Slot
	{
		AEngineComponent* ptr = nullptr;
		uint32_t generation = 1;
	};

	static ComponentStorage& Get();

	ComponentHandle Register(AEngineComponent* component);

	void UnRegister(ComponentHandle handle);

	AEngineComponent* Get(ComponentHandle handle);
private:
	std::vector<Slot> slots;
};