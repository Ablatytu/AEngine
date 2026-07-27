#pragma once
#include <string>
#include <memory>
#include "../core/AEngineAPI.h"
class Material;

class MaterialSerializer
{
public:
	static bool Serialize(Material& material, const std::string& path);
	static std::unique_ptr<Material> Deserialize(const std::string& path);
	AENGINE_API static bool IsMaterial(const std::string& path);
};