#ifdef __APPLE__
#include "DynamicLibrary.h"

#include <dlfcn.h>
#include <unistd.h>
#include <mach-o/dyld.h>

namespace Platform
{
	namespace DynamicLibrary
	{
		void* Load(const std::filesystem::path& path)
		{
			return dlopen(path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
		}
		void* GetSymbol(void* lib, const char* name)
		{
			return dlsym(lib, name);
		}
		bool UnloadLibrary(void* lib)
		{
			return dlclose(lib) == 0;
		}
		uint64_t GetProcessId()
		{
			return static_cast<uint64_t>(getpid());
		}
	}
}
#endif 