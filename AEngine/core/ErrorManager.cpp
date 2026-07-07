#include "ErrorManager.h"
#include <stdlib.h>
#include <Windows.h>

void CriticalGameError(const char* errorname)
{
	glfwHideWindow(Application::Get().GetWindow().window);
	MessageBoxA(NULL, errorname, "Critical error", MB_ICONERROR | MB_OK);
	glfwTerminate();
	glfwSetWindowShouldClose(Application::Get().GetWindow().window, true);
	exit(0);
	return;
}