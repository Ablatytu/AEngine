#pragma once
#include <filesystem>
#include "../core/AEngineAPI.h"
#include "../Renderer/GameObjectHandle.h"

struct AE_API;
struct AE_COMPONENT_INFO;
struct SavedComponentInfo;
struct RenderContext;

namespace GameModule
{
	AENGINE_API bool Load(const std::filesystem::path& dllPath);
	AENGINE_API bool Unload();
	AENGINE_API bool Reload();
	
	AENGINE_API bool OnRenderImage(RenderContext& context, const char* source, const char* destionation);
	AENGINE_API void Update(float deltaTime);
	AENGINE_API void Start();
	AENGINE_API AE_GAMEAPI GETGAMECONTEXT();
	void DeleteOldShadowCopies(const std::filesystem::path& directory);
	void RestoreGameAComponents(std::vector<SavedComponentInfo>& components_storage);
	void SaveOldGameAComponents(std::vector<AE_COMPONENT_INFO>& components_storage, std::vector<SavedComponentInfo>& toSave);
	//AENGINE_API void CheckForChanges();
}