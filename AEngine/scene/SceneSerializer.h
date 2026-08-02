#pragma once
#include <string>
#include <memory>
#include "../core/AEngineAPI.h"
class Scene;

class SceneSerializer
{
public:
	AENGINE_API static bool SerializeEmpty(const std::string& path);
	AENGINE_API static bool Serialize(Scene& scene, const std::string& path);
	AENGINE_API static std::unique_ptr<Scene> Deserialize(const std::string& path);
};