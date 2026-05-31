#include "ErrorManager.h"
#include <stdlib.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <Windows.h>

void CriticalGameError(const char* errorname)
{
	glfwHideWindow(Application::Get().window.window);
	MessageBoxA(NULL, errorname, "Critical error", MB_ICONERROR | MB_OK);
	glfwTerminate();
	glfwSetWindowShouldClose(Application::Get().window.window, true);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	exit(0);
	return;
}