#include "ComponentSystem.h"
#include "../components/AEngineComponent.h"
ComponentRegistry& ComponentSystem::GetRegistry()
{
	static ComponentRegistry compreg;
	return compreg;
}
void ComponentSystem::Register(const char* name, std::function<AEngineComponent*(GameObject*)> addFunc)
{
	GetRegistry().addComponent[name] = addFunc;
}
std::unordered_map<std::string, std::function<AEngineComponent*(GameObject* gameobject)>>& ComponentSystem::GetComponentsFabric()
{
	return GetRegistry().addComponent;
}
