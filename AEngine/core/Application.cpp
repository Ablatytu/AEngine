#include "Application.h"
#include "../Renderer/GameObject.h"
#include "Window.h"
#include <map>
#include <cmath>
#include "../Renderer/RenderSystem.h"
#include "../components/GameModule.h"
#include "../scene/SceneManager.h"
#include "../scene/Scene.h"
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
const bool Application::GetUpdateGameModuleComponents() const
{
	return UpdateGameModuleComponents;
}
void Application::UpdateGameModule(bool update)
{
	UpdateGameModuleComponents = update;
}
int Application::run()
{
	OnInit();
	while (!glfwWindowShouldClose(window.window))
	{
		OnStartFrame();
		glfwPollEvents();
		Time::UpdateTime();
		OnBeforeSceneUpdate();
		if (!SceneManager::Get().GetActiveScene().StartAComponents.empty())
		{
			for (auto& component : SceneManager::Get().GetActiveScene().StartAComponents)
			{
				if (!component)
					continue;
				if (!component->gameObject)
					continue;
				if (component->gameObject->isDestroying)
					continue;
				component->Start();
			}
			SceneManager::Get().GetActiveScene().StartAComponents.clear();
		}
		if(UpdateGameModuleComponents)
			GameModule::Start();
		for (auto& component : SceneManager::Get().GetActiveScene().AComponents)
		{
			if (!component)
				continue;
			if (!component->gameObject)
				continue;
			if (component->gameObject->isDestroying)
				continue;
			component->Update();
		}
		if (UpdateGameModuleComponents)
			GameModule::Update(Time::deltaTime);
		SceneManager::Get().GetActiveScene().DestroyDestroyingObjects();
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