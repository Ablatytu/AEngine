#include "Window.h"
#include "ErrorManager.h"
#include "../Renderer/FramebufferManager.h"
#include "../Renderer/Buffer.h"

Window* Window::GlobalWindow = nullptr;
bool Input::firstMouse = true;
float Input::xoffset = 0.0f;
float Input::yoffset = 0.0f;
double Input::xpos = 0.0;
double Input::ypos = 0.0;
float Input::lastX = 0.0;
float Input::lastY = 0.0f;
float Input::scrollYOffset = 0.0f;
float Input::viewportX = 0.0f;
float Input::viewportY = 0.0f;
float Input::viewportWidth = 0.0f;
float Input::viewportHeight = 0.0f;
static bool keysDown[512];
static bool keysUp[512];
static bool keys[512];
static bool mouseButtons[3];
static bool mouseButtonsUp[3];
static bool mouseButtonsDown[3];
float Time::lastTime = 0.0f;
float Time::deltaTime = 0.0f;
void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollYOffset = yoffset;
}
void Input::SetViewport(float x, float y, float width, float height)
{
	viewportX = x; viewportY = y;
	viewportWidth = std::max(1.0f, width); viewportHeight = std::max(1.0f, height);
}
float Input::GetMouseX()
{
	return xpos - viewportX;
}
float Input::GetMouseY()
{
	return ypos - viewportY;
}
void Input::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	Input::xpos = static_cast<float>(xposIn);
	Input::ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	xoffset = lastX - xpos;
	yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
}
void Input::mouseButtonCallback(GLFWwindow* window, int key, int action, int mods)
{
	if (key < 0 || key > 2)
		return;
	if (action == GLFW_PRESS)
	{
		if (!mouseButtons[key])
			mouseButtonsDown[key] = true;
		mouseButtons[key] = true;
		mouseButtonsUp[key] = false;
	}
	else if (action == GLFW_RELEASE)
	{
		mouseButtonsUp[key] = true;
		mouseButtons[key] = false;
	}
}
void Input::keyButton_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key < 0 || key >= 512)
		return;
	if (action == GLFW_PRESS)
	{
		if (!keys[key])
			keysDown[key] = true;
		keys[key] = true;
		keysUp[key] = false;
	}
	else if (action == GLFW_RELEASE)
	{
		keysUp[key] = true;
		keys[key] = false;
	}
}
void Window_size(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Window::GlobalWindow->ChangeScreenSize((float)width, (float)height);
}
Window::Window()
{
	std::cout << "CREATING WINDOW\n";
	GlobalWindow = this;
	Input::xpos = screenX / 2;
	Input::ypos = screenY / 2;
	Input::lastX = screenX / 2;
	Input::lastY = screenY / 2;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//glfwWindowHint(GLFW_SAMPLES, 4);
	const char* nameWindow = "AEngine";
#ifdef _DEBUG
	nameWindow = "AEngine(Debug)";
#endif

	window = glfwCreateWindow(screenX, screenY, nameWindow, NULL, NULL);
	if (window == NULL)
	{
		CriticalGameError("Failed to create window");
		return;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		CriticalGameError("OpenGL error");
		return;
	}
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, screenX, screenY);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, Window_size);
	glfwSetCursorPosCallback(window, Input::mouse_callback);
	glfwSetScrollCallback(window, Input::scroll_callback);
	glfwSetKeyCallback(window, Input::keyButton_callback);
	glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
}
void Window::ChangeScreenSize(float width, float height)
{
	screenX = width;
	screenY = height;
}
float Input::GetAxis(const char* axis)
{
	if (strcmp(axis, "Mouse X") == 0)
	{
		float temp = xoffset;
		xoffset = 0.0f;
		return temp;
	}
	else if (strcmp(axis, "Mouse Y") == 0)
	{
		float temp = yoffset;
		yoffset = 0.0f;
		return temp;
	}
	else if (strcmp(axis, "Horizontal") == 0)
	{
		float value = 0.0f;
		if (glfwGetKey(Window::GlobalWindow->window, GLFW_KEY_D) == GLFW_PRESS) value += 1.0f;
		if (glfwGetKey(Window::GlobalWindow->window, GLFW_KEY_A) == GLFW_PRESS) value -= 1.0f;
		return value;
	}
	else if (strcmp(axis, "Vertical") == 0)
	{
		float value = 0.0f;
		if (glfwGetKey(Window::GlobalWindow->window, GLFW_KEY_W) == GLFW_PRESS) value += 1.0f;
		if (glfwGetKey(Window::GlobalWindow->window, GLFW_KEY_S) == GLFW_PRESS) value -= 1.0f;
		return value;
	}
	else if (strcmp(axis, "Mouse ScrollWheel") == 0)
	{
		float temp = scrollYOffset;
		scrollYOffset = 0.0f;
		return temp;
	}
	return 0.0f;
}
bool Input::GetMouseButton(int button)
{
	if (button < 0 || button > 2)
		return false;
	return mouseButtons[button];
}
bool Input::GetMouseButtonUp(int button)
{
	if (button < 0 || button > 2)
		return false;
	bool up = mouseButtonsUp[button];
	mouseButtonsUp[button] = false;
	return up;
}
bool Input::GetMouseButtonDown(int button)
{
	if (button < 0 || button > 2)
		return false;
	bool down = mouseButtonsDown[button];
	mouseButtonsDown[button] = false;
	return down;
}
bool Input::GetKeyUp(int button)
{
	if (button >= 0 && button < 512)
	{
		bool up = keysUp[button];
		keysUp[button] = false;
		return up;
	}
	return false;
}
bool Input::GetKey(int button)
{
	if (button >= 0 && button < 512)
		return keys[button];
	return false;
}
bool Input::GetKeyDown(int button)
{
	if (button >= 0 && button < 512)
	{
		bool down = keysDown[button];
		keysDown[button] = false;
		return down;
	}
	return false;
}
Time::u_Time Time::shader_Time;
void Time::UpdateTime()
{
	float currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	shader_Time._Time = currentTime;
	UBOBuffer::ChangeInfoBuffer("u_Time", sizeof(u_Time), &shader_Time, 0);
}