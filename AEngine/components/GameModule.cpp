#include "GameModule.h"
#include "../core/ErrorManager.h"
#include "../scene/SceneManager.h"
#include "../core/ComponentSystem.h"
#include "../scene/Scene.h"
#include "../../GameAPI/GameABI.h"
#include "../Renderer/GameObjectHandle.h"
#include "EngineEntry.h"
#ifdef __APPLE__
	#include <dlfcn.h>
	#include <unistd.h>
	#include <mach-o/dyld.h>
#endif

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "../Platform/DynamicLibrary.h"

#include <iostream>

namespace GameModule
{
	static void* module = nullptr;
	static std::filesystem::path loadedDll;
	static std::filesystem::path sourceDll;
	static unsigned generation = 0;
	static bool hasWriteTime = false;
	static std::filesystem::file_time_type lastWriteTime;
	const uint64_t processID = Platform::DynamicLibrary::GetProcessId();
	static AE_API ae_api{};
	static AE_GAMEAPI game_api{};

	struct SavedFields
	{
		int valuei = 0;
		float valuef = 0.0f;
		AE_VEC3 value3;
		AE_VEC4 value4;
		bool valueb = false;

		AE_GAME_FIELD_INFO::FieldType typeValue = AE_GAME_FIELD_INFO::FieldType::INT;
		std::string name = "";
	};
	struct SavedComponentInfo
	{
		AE_GAMEOBJECT_HANDLE handle{};

		std::string componentName = "";

		std::vector<SavedFields> savedFields;
	};

	AE_GAMEAPI GETGAMECONTEXT()
	{
		return game_api;
	}
	void DeleteOldShadowCopies(const std::filesystem::path& directory)
	{
		std::error_code iteratorError;

		for (const auto& entry : std::filesystem::directory_iterator(directory, iteratorError))
		{
			if (iteratorError)
			{
				CriticalGameError("FAILED TO OPEN DIRECTORY");
				break;
			}
			if (!entry.is_regular_file())
				continue;
			const std::string filename = entry.path().filename().string();

	#ifdef _WIN32
			const std::string extension = ".dll";
	#endif

	#ifdef __APPLE__
			const std::string extension = ".dylib";
	#endif

			const bool isShadowGameDll = filename.rfind("Game_SHADOW_", 0) == 0 && entry.path().extension() == extension;

			if (!isShadowGameDll)
				continue;

			std::error_code removeError;
			std::filesystem::remove(entry.path(), removeError);

			if (!removeError)
			{
				std::cout << "Deleted old shadow dll: " << entry.path() << "\n";
			}
		}
	}
	void SaveOldGameAComponents(std::vector<AE_COMPONENT_INFO>& components_storage, std::vector<SavedComponentInfo>& toSave)
	{
		toSave.clear();
		for (AE_COMPONENT_INFO& ae_component_old_info : components_storage)
		{
			if (!ae_component_old_info.name || ae_component_old_info.name[0] == '\0') continue;
			std::string nameComponent = ae_component_old_info.name;
			AE_GAMEOBJECT_HANDLE gameObjectHandle = ae_component_old_info.ae_gameObject_handle;

			std::vector<SavedFields> savedFields(ae_component_old_info.countFieldsInfo);
			for (int i = 0; i < ae_component_old_info.countFieldsInfo; i++)
			{
				savedFields[i].name = ae_component_old_info.fieldsInfo[i].nameField;
				savedFields[i].typeValue = ae_component_old_info.fieldsInfo[i].typeValue;

				switch (ae_component_old_info.fieldsInfo[i].typeValue)
				{
					case AE_GAME_FIELD_INFO::FieldType::BOOL:
					{
						savedFields[i].valueb = *static_cast<bool*>(ae_component_old_info.fieldsInfo[i].ptr);
						break;
					}
					case AE_GAME_FIELD_INFO::FieldType::INT:
					{
						savedFields[i].valuei = *static_cast<int*>(ae_component_old_info.fieldsInfo[i].ptr);
						break;
					}
					case AE_GAME_FIELD_INFO::FieldType::FLOAT:
					{
						savedFields[i].valuef = *static_cast<float*>(ae_component_old_info.fieldsInfo[i].ptr);
						break;
					}
					case AE_GAME_FIELD_INFO::FieldType::VEC3:
					{
						savedFields[i].value3 = *static_cast<AE_VEC3*>(ae_component_old_info.fieldsInfo[i].ptr);
						break;
					}
					case AE_GAME_FIELD_INFO::FieldType::VEC4:
					{
						savedFields[i].value4 = *static_cast<AE_VEC4*>(ae_component_old_info.fieldsInfo[i].ptr);
						break;
					}
				}
			}
			toSave.push_back({ gameObjectHandle, nameComponent, savedFields });
		}
	}
	void RestoreGameAComponents(std::vector<SavedComponentInfo>& components_storage)
	{
		int count = game_api.getRegisteredAComponentsCount();
		for (SavedComponentInfo& components_old : components_storage)
		{
			for (int i = 0; i < count; i++)
			{
				const char* registeredName = game_api.getRegisteredAComponentName(i);

				if (registeredName && components_old.componentName == registeredName)
				{
					game_api.addAComponent(i, components_old.handle);
					break;
				}
			}
		}
		int newComponentsCount = game_api.getAComponentCount();
		std::vector<AE_COMPONENT_INFO> new_ae_component_info(newComponentsCount);
		game_api.getAComponentsInfo(new_ae_component_info.data(), newComponentsCount);
		for (SavedComponentInfo& components_old : components_storage)
		{
			for (AE_COMPONENT_INFO& new_info : new_ae_component_info)
			{
				if (!new_info.name) continue;

				if (new_info.ae_gameObject_handle.generation != components_old.handle.generation || new_info.ae_gameObject_handle.index != components_old.handle.index) continue;

				if (new_info.name != components_old.componentName) continue;

				for (SavedFields& oldField : components_old.savedFields)
				{
					for (int i = 0; i < new_info.countFieldsInfo; i++)
					{

						AE_GAME_FIELD_INFO::FieldInfo& newField = new_info.fieldsInfo[i];

						if (!newField.nameField || !newField.ptr) continue;

						if (oldField.name != newField.nameField || oldField.typeValue != newField.typeValue) continue;

						switch (new_info.fieldsInfo[i].typeValue)
						{
							case AE_GAME_FIELD_INFO::FieldType::BOOL:
							{
								*static_cast<bool*>(new_info.fieldsInfo[i].ptr) = oldField.valueb;
								break;
							}
							case AE_GAME_FIELD_INFO::FieldType::INT:
							{
								*static_cast<int*>(new_info.fieldsInfo[i].ptr) = oldField.valuei;
								break;
							}
							case AE_GAME_FIELD_INFO::FieldType::FLOAT:
							{
								*static_cast<float*>(new_info.fieldsInfo[i].ptr) = oldField.valuef;
								break;
							}
							case AE_GAME_FIELD_INFO::FieldType::VEC3:
							{
								*static_cast<AE_VEC3*>(new_info.fieldsInfo[i].ptr) = oldField.value3;
								break;
							}
							case AE_GAME_FIELD_INFO::FieldType::VEC4:
							{
								*static_cast<AE_VEC4*>(new_info.fieldsInfo[i].ptr) = oldField.value4;
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
	}
	bool LoadGameAPI()
	{
		AE_GAME_API_FN gameAPI = reinterpret_cast<AE_GAME_API_FN>(Platform::DynamicLibrary::GetSymbol(module, "AE_GAME_API"));

		if (!gameAPI)
		{
			CriticalGameError("GAME API NOT FINDED");
			game_api = {};
			ae_api = {};
			return false;
		}
		ae_api = {};
		game_api = {};

		EngineAPI::BuildAPI(ae_api);

		if (!gameAPI(&ae_api, &game_api))
		{
			CriticalGameError("FAILED TO LOAD GAMEAPI");
			return false;
		}
		if (!game_api.shutdown)
		{
			CriticalGameError("SHUTDOWN NOT FINDED");
			return false;
		}
		return true;
	}
	bool Load(const std::filesystem::path& dllPath)
	{
		if (module)
			return true;

	#ifdef __APPLE__
		uint32_t size = 0;

		_NSGetExecutablePath(nullptr, &size);

		std::string buffer(size, '\0');

		_NSGetExecutablePath(buffer.data(), &size);
		
		loadedDll = std::filesystem::weakly_canonical(buffer).parent_path() / ("Game_" + std::string("SHADOW_") + std::to_string(processID) + "_" + std::to_string(++generation) + ".dylib");

		std::filesystem::path exeDirectory = std::filesystem::path(buffer).parent_path();
		sourceDll = dllPath.is_absolute() ? dllPath : exeDirectory / dllPath;
	#endif

	#ifdef _WIN32
		wchar_t path[MAX_PATH];

		GetModuleFileNameW(nullptr, path, MAX_PATH);

		std::filesystem::path exeDirectory = std::filesystem::path(path).parent_path();
		if (generation == 0)
		{
			DeleteOldShadowCopies(exeDirectory);
		}

		sourceDll = dllPath.is_absolute() ? dllPath : exeDirectory / dllPath;

		loadedDll = exeDirectory / ("Game_" + std::string("SHADOW_") + std::to_string(processID) + "_" + std::to_string(++generation) + ".dll");
	#endif

		std::error_code error;
		std::filesystem::copy_file(sourceDll, loadedDll, std::filesystem::copy_options::overwrite_existing, error);
		if (error)
		{
			CriticalGameError((std::string("FAILED TO CREATE COPY GAME.DLL: ") + error.message()).c_str());
			return false;
		}
		module = Platform::DynamicLibrary::Load(loadedDll.c_str());
		if (!module)
		{
			CriticalGameError("FAILED TO LOAD GAME.DLL");
			return false;
		}
		if (!LoadGameAPI())
		{
			game_api = {};
			ae_api = {};

			Platform::DynamicLibrary::UnloadLibrary(module);
			module = nullptr;

			return false;
		}
		lastWriteTime = std::filesystem::last_write_time(sourceDll, error);
		hasWriteTime = !error;
		std::cout << "Game.dll loaded\n";
		return true;
	}

	bool Unload()
	{
		if (!module)
			return true;

		if (game_api.shutdown)
			game_api.shutdown();

		game_api = {};

		if (!Platform::DynamicLibrary::UnloadLibrary(module))
		{
			CriticalGameError("FAILED TO UNLOAD GAME.DLL");
			return false;
		}

		module = nullptr;

		std::error_code error;
		std::filesystem::remove(loadedDll, error);
		if (error)
		{
			CriticalGameError((std::string("FAILED TO DELETE COPY GAME.DLL: ") + error.message()).c_str());
			return false;
		}
		loadedDll.clear();
		std::cout << "Game.dll unloaded\n";
		return true;
	}
	bool Reload()
	{
		if (sourceDll.empty())
			return false;
		int countInfo = game_api.getAComponentCount();
		std::vector<AE_COMPONENT_INFO> componentsInfoSaved(countInfo);
		game_api.getAComponentsInfo(componentsInfoSaved.data(), countInfo);
		std::vector<SavedComponentInfo> savedOldFields;
		SaveOldGameAComponents(componentsInfoSaved, savedOldFields);
		if (!Unload())
			return false;
		if (!Load(sourceDll))
			return false;
		RestoreGameAComponents(savedOldFields);
		return true;
	}

	void Start()
	{
		if (!module)
			return;
		if (!game_api.start)
			return;

		game_api.start();
	}
	void Update(float deltaTime)
	{
		if (!module)
			return;
		if (!game_api.update)
			return;

		game_api.update(deltaTime);
	}
	//void CheckForChanges()
	//{
	//	if (sourceDll.empty())
	//		return;
	//	if (!module)
	//	{
	//		Load(sourceDll);
	//		return;
	//	}
	//	std::error_code error;

	//	const auto currentWriteTime = std::filesystem::last_write_time(sourceDll, error);

	//	if (error)
	//		return;
	//	if (!hasWriteTime)
	//	{
	//		lastWriteTime = currentWriteTime;
	//		hasWriteTime = true;
	//		return;
	//	}

	//	if (currentWriteTime != lastWriteTime)
	//	{
	//		std::cout << "Game.dll changed\n";

	//		Reload();
	//	}

	//}
}