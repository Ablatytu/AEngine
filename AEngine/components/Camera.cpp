#include "../Renderer/GameObject.h"
#include "Camera.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "../scene/SceneManager.h"
#include "../core/Application.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Buffer.h"
#include "../Renderer/RenderSystem.h"

void Camera::Start()
{
	transform = gameObject->GetComponent<Transform>();
	viewMatrix = glm::lookAt(transform->position, transform->position + transform->forward(), transform->up());
	RecultivateProjection();
	RenderSystem::rs_CreateCameraFramebuffers(*this);
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

Framebuffer* Camera::Add(std::string name, Shader* frameshader, const std::vector<Framebuffer::TypeFrameBuffer>& typeFrameBuffer, bool resizable, int width, int height, const std::vector<Framebuffer::TypeFilter>& typeFilter)
{
	std::unique_ptr<Framebuffer> framebuffer = std::make_unique<Framebuffer>(frameshader, typeFrameBuffer, resizable, width, height, typeFilter);
	Framebuffer* framebufferptr = framebuffer.get();
	framebuffers[name] = std::move(framebuffer);
	return framebufferptr;
}
void Camera::Resize(int width, int height)
{
	for (auto& it : framebuffers)
		it.second->Resize(width, height);
}
void Camera::Delete(std::string name)
{
	auto it = framebuffers.find(name);
	if (it != framebuffers.end())
		framebuffers.erase(it);
}
Framebuffer* Camera::GetFramebuffer(std::string name)
{
	auto it = framebuffers.find(name);
	if (it != framebuffers.end())
		return it->second.get();
	return nullptr;
}
std::vector<Framebuffer*> Camera::GetFramebuffers()
{
	std::vector<Framebuffer*> buffers;
	for (auto& buffer : framebuffers)
	{
		buffers.push_back(buffer.second.get());
	}
	return buffers;
}
Camera::~Camera()
{
	framebuffers.clear();
}
AE_REGISTER_COMPONENT(Camera)
