#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "../GameAPI/GameABI.h"
class ComponentSystem
{
public:
	static ComponentSystem& Get();
	void Register(const char* name, std::function<AE_COMPONENT_HANDLE(AE_GAMEOBJECT_HANDLE)> addComponent);
	const char* GetAComponentName(unsigned int ID);
	const int GetAComponentCount() const;
	void AddAComponent(unsigned int ID, AE_GAMEOBJECT_HANDLE ae_gameObject_handle);
	void RemoveAllGameAComponents();
private:
	struct ComponentInfo
	{
		std::string name = "";
		std::function<AE_COMPONENT_HANDLE(AE_GAMEOBJECT_HANDLE)> addComponent;
	};
	std::vector<ComponentInfo> components;
};