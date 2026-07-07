#include "Application.h"
#include "../Renderer/GameObject.h"
#include "Window.h"
#include <map>
#include <cmath>
#include "../Renderer/RenderSystem.h"
using namespace RenderSystem;
Application* Application::app = nullptr;

Application::Application()
{
	if (!app)
		app = this;
}
const Window& Application::GetWindow() const
{
	return window;
}
Application& Application::Get()
{
	return *app;
}
Window& Application::GetWindow()
{
	return window;
}
int Application::run()
{
	OnInit();
	while (!glfwWindowShouldClose(window.window))
	{
		glfwPollEvents();
		Time::UpdateTime();
		OnBeforeSceneUpdate();

		if (!SceneManager::Get().GetActiveScene().AwakeAComponents.empty())
		{
			for (auto& component : SceneManager::Get().GetActiveScene().AwakeAComponents)
				component->Awake();
			SceneManager::Get().GetActiveScene().AwakeAComponents.clear();
		}
		if (!SceneManager::Get().GetActiveScene().StartAComponents.empty())
		{
			for (auto& component : SceneManager::Get().GetActiveScene().StartAComponents)
				component->Start();
			SceneManager::Get().GetActiveScene().StartAComponents.clear();
		}
		for (auto& component : SceneManager::Get().GetActiveScene().AComponents)
		{
			component->Update();
		}
		OnAfterSceneUpdate();

		OnRender();
		glfwSwapBuffers(window.window);
		rs_InitAfterFirstFrame();
	}
	OnShutdown();
	glfwTerminate();
	return 0;
}
void Application::close()
{
	glfwSetWindowShouldClose(window.window, true);
}