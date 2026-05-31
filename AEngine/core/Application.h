#ifndef APPLICATION_H
#define APPLICATION_H
#include "Window.h"
#include <string>
class Application
{
public:
	Application();
	static Application& Get();
	Window window;
	int run();
	int close();
private: 
	static Application* app;
};
#endif

