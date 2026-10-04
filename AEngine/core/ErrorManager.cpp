#include "ErrorManager.h"
#include <stdlib.h>

#ifdef _WIN32
	#include <Windows.h>
#endif

void CriticalGameError(const char* errorname)
{
	glfwHideWindow(Application::Get().GetWindow().window);
#ifdef _WIN32
	MessageBoxA(NULL, errorname, "Critical error", MB_ICONERROR | MB_OK);
#endif

#ifdef __APPLE__
	std::cerr << "\033[31m" << "Critical error: " << errorname << "\033[0m\n";
#endif
	glfwTerminate();
	glfwSetWindowShouldClose(Application::Get().GetWindow().window, true);
	exit(0);
	return;
}