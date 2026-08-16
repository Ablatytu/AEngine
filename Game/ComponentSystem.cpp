#include "ComponentSystem.h"

ComponentSystem& ComponentSystem::Get()
{
	static ComponentSystem componentSystemInstance;
	return componentSystemInstance;
}

void ComponentSystem::Register(const char* name, std::function<AE_COMPONENT_HANDLE(AE_GAMEOBJECT_HANDLE)> addComponent)
{
	Get().components.push_back({ name, addComponent });
}

const char* ComponentSystem::GetAComponentName(unsigned int ID)
{
	if (ID >= components.size()) return {};
	return Get().components[ID].name.c_str();
}

const int ComponentSystem::GetAComponentCount() const
{
	return Get().components.size();
}

void ComponentSystem::AddAComponent(unsigned int ID, AE_GAMEOBJECT_HANDLE ae_gameObject_handle)
{
	if (ID >= components.size()) return;
	Get().components[ID].addComponent(ae_gameObject_handle);
}

void ComponentSystem::RemoveAllGameAComponents()
{
	components.clear();
}