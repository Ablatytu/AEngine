#ifndef WINDOW_H 
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Editor/Editor.h"
class Window
{
public:
	GLFWwindow* window;
	static Window* GlobalWindow;
	float screenX = 800.0f;
	float screenY = 600.0f;
	float lastX = screenX / 2.0f;
	EditorGUI* editorGUI;
	Window();

	void ChangeScreenSize(float width, float height);
};
class Input
{
public:
	static double xpos, ypos;
	static float yaw, pitch, lastX, lastY, xoffset, yoffset, scrollYOffset;
	static bool firstMouse;
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static float GetAxis(const char* axis);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static bool GetKey(int button);
	static bool GetKeyUp(int button);
	static bool GetMouseButton(int button);
	static bool GetKeyDown(int button);
	static void keyButton_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static bool GetMouseButtonDown(int button);
	static bool GetMouseButtonUp(int button);
};
class Time
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