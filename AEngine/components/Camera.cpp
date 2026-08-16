#include "../Renderer/GameObject.h"
#include "Camera.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "../scene/SceneManager.h"
#include "../core/Application.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Buffer.h"

void Camera::Start()
{
	transform = gameObject->GetComponent<Transform>();
	viewMatrix = glm::lookAt(transform->position, transform->position + transform->forward(), transform->up());
	RecultivateProjection();
}
void Camera::SetViewport(int width, int height)
{
	ratio = (float)width / (float)height;
	RecultivateProjection();
}
void Camera::RecultivateProjection()
{
	projection = glm::perspective(glm::radians(fieldOView), ratio, near, far);
}
void Camera::Update()
{
	viewMatrix = glm::lookAt(transform->position, transform->position + transform->forward(), transform->up());
}

AE_REGISTER_COMPONENT(Camera)
