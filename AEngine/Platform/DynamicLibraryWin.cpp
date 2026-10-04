#ifdef _WIN32
#include "DynamicLibrary.h"
#include <Windows.h>

namespace Platform
{
	namespace DynamicLibrary
	{
		void* Load(const std::filesystem::path& path)
		{
			return reinterpret_cast<void*>(LoadLibraryW(path.c_str()));
		}
		void* GetSymbol(void* lib, const char* name)
		{
			return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(lib), name));
		}
		bool UnloadLibrary(void* lib)
		{
			return FreeLibrary(reinterpret_cast<HMODULE>(lib));
		}
		uint64_t GetProcessId()
		{
			return static_cast<uint64_t>(GetCurrentProcessId());
		}
	}
}
#endif