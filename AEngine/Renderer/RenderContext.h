#pragma once
#include <vector>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
class Camera;
class Render;
class Light;
class Skybox;
class Editor;
class Window;
class PostProcess;

struct FogSettings
{
	bool enabled = true;;
	glm::vec3 fogColor = glm::vec3(1.0);
};

class RenderContext
{
public:
	Camera* camera = nullptr;
	std::vector<Render*> renders;
	FogSettings fog{};
	std::vector<Light*> lights;
	Skybox* skybox = nullptr;
	Window* window;
	Render* selectedObject = nullptr;
	PostProcess* postProcess;
	bool HDR = false;
};