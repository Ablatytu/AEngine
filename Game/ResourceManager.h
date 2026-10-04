#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "EngineAPIContext.h"

class ResourceManager
{
public:
	template<typename typeResource> static typeResource Load(const char*, const char* = nullptr);

	template<> static Texture Load<Texture>(const char* name, const char* name1)
	{
		if (!name)
			return {};
		const AE_API* ae_api = ENGINEAPICONTEXT::Get();
		if (!ae_api)
			return {};

		AE_GAME_RESOURCES_INFO::AE_TEXTURE findedTexture{}; 
		if (ae_api->getTexture(name, &findedTexture))
			return Texture(findedTexture);
		return {};
	}
	template<> static Shader Load<Shader>(const char* vertexPath, const char* fragmentPath)
	{
		if (!fragmentPath || !vertexPath)
			return {};

		const AE_API* ae_api = ENGINEAPICONTEXT::Get();
		if (!ae_api)
			return {};

		AE_GAME_RESOURCES_INFO::AE_SHADER loadedShader{};
		if (ae_api->getShader(vertexPath, fragmentPath, &loadedShader))
			return loadedShader;
		return {};
	}
	template<> static Material Load<Material>(const char* name, const char* name1)
	{
		if (!name)
			return {};
		const AE_API* ae_api = ENGINEAPICONTEXT::Get();
		if (!ae_api)
			return {};
		
		AE_GAME_RESOURCES_INFO::AE_MATERIAL findedMaterial{};
		if (ae_api->getMaterial(name, &findedMaterial))
			return Material(findedMaterial);
		return {};
	}
	
};