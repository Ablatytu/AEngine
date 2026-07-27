#pragma once
#include <string>
#include <memory>
#include "../core/AEngineAPI.h"

class Cubemap;

class CubemapSerializer
{
public:
	static bool Serialize(const Cubemap& cubemap, const std::string& path);
	static std::unique_ptr<Cubemap> Deserialize(const std::string& path);
	AENGINE_API static bool IsCubemap(const std::string& path);
};