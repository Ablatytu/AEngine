#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "../core/AEngineAPI.h"

class GameObject; 
class AEngineComponent;

struct ComponentRegistry
{
	std::unordered_map<std::string, std::function<AEngineComponent*(GameObject* gameobject)>> addComponent;
};
class ComponentSystem
{
public:

	AENGINE_API static void Register(const char* name, std::function<AEngineComponent* (GameObject*)> addFunc);
	AENGINE_API static std::unordered_map<std::string, std::function<AEngineComponent*(GameObject* gameobject)>>& GetComponentsFabric();
private:
	static ComponentRegistry& GetRegistry();
};