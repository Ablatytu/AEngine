#pragma once
#include <vector>
#include "../components/AComponent.h"
#include <memory>
struct ComponentEntry
{
	const char* name;
	//std::function<std::shared_ptr<AComponent>()> create;
};
//inline std::vector<ComponentEntry>& GetComponentRegistry() \
{\
	static std::vector<ComponentEntry> registry;\
	return registry;\
}
class ComponentSystem
{
public:
	//static std::vector<AComponent*> componentsList;
	//static void RegisterComponentInList(AComponent* component);
};