#ifndef APPLICATION_H
#define APPLICATION_H
#include "Window.h"
#include <string>
#include "AEngineAPI.h"


class AENGINE_API Application
{
public:
	Application();
	static Application& Get();
	virtual ~Application() = default;
	virtual void OnSceneLoaded() {};
	const bool GetUpdateGameModuleComponents() const;
	void UpdateGameModule(bool update);
	int run();
	void close();

	const Window& GetWindow() const;
	Window& GetWindow();
private:

	virtual void OnInit() {}
	virtual void OnShutdown() {}

	virtual void OnStartFrame() {};
	virtual void OnBeforeSceneUpdate() {}
	virtual void OnAfterSceneUpdate() {}

	virtual void OnRender() {}

private: 
	static Application* app;
	Window window;
	bool UpdateGameModuleComponents = false;
};
#endif

