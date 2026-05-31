#include "Camera.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "../Renderer/GameObject.h"
#include "../scene/SceneManager.h"
#include "../core/Application.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Buffer.h"

void Camera::Start()
{
	transform = gameObject->GetComponent<Transform>();
	viewMatrix = glm::lookAt(transform->position, transform->position + transform->forward(), transform->up());
	projection = glm::perspective(glm::radians(fieldOView), (float)Application::Get().window.screenX / (float)Application::Get().window.screenY, near, far);
}
void Camera::Update()
{
	viewMatrix = glm::lookAt(transform->position, transform->position + transform->forward(), transform->up());
	projection = glm::perspective(glm::radians(fieldOView), (float)Application::Get().window.screenX / (float)Application::Get().window.screenY, near, far);
}
