#include <filesystem>
#include <cstdint>

namespace Platform
{
	namespace DynamicLibrary
	{
		void* Load(const std::filesystem::path& path);
		void* GetSymbol(void* lib, const char* name);
		bool UnloadLibrary(void* lib);
		uint64_t GetProcessId();
	};
};