#ifndef WINDOW_H 
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "AEngineAPI.h"

class Framebuffer;

class AENGINE_API Window
{
public:
	GLFWwindow* window;
	static Window* GlobalWindow;
	float screenX = 1280.0f;
	float screenY = 720.0f;
	float lastX = screenX / 2.0f;
	Window();

	void ChangeScreenSize(float width, float height);
};
class Input
{
public:
	AENGINE_API static void SetViewport(float x, float y, float width, float height);
	AENGINE_API static float GetMouseX();
	AENGINE_API static float GetMouseY();
	AENGINE_API static double xpos, ypos;
	AENGINE_API static float yaw, pitch, lastX, lastY, xoffset, yoffset, scrollYOffset;
	AENGINE_API static bool firstMouse;
	AENGINE_API static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	AENGINE_API static float GetAxis(const char* axis);
	AENGINE_API static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	AENGINE_API static bool GetKey(int button);
	AENGINE_API static bool GetKeyUp(int button);
	AENGINE_API static bool GetMouseButton(int button);
	AENGINE_API static bool GetKeyDown(int button);
	AENGINE_API static void keyButton_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	AENGINE_API static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	AENGINE_API static bool GetMouseButtonDown(int button);
	AENGINE_API static bool GetMouseButtonUp(int button);
	AENGINE_API static void Reset();
private:
	static float viewportX;
	static float viewportY;
	static float viewportWidth;
	static float viewportHeight;
};
class AENGINE_API Time
{
public:
	static float deltaTime;
	static void UpdateTime();
private:
	static float lastTime; 
private:
	struct u_Time
	{
		float _Time;
		float padding[3];
	};
	static u_Time shader_Time;
};


#endif