#pragma once
#include "AEngineComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "../components/Transform.h"
#include "../core/AEngineAPI.h"
#include "../Renderer/Framebuffer.h"

class Material;

class Camera : public AEngineComponent
{
	AE_REGISTER_CLASS(Camera);
public:
	enum class ProjectionType
	{
		Perspective,
		Orthographic
	};

	ProjectionType projectionType = ProjectionType::Perspective;

	float orthographicSize = 4.0f;

	float fieldOView = 60.0f;
	Transform* transform;
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	float near = 0.1f;
	float far = 100.0f;
	float ratio = 1.0f;
	bool exposurePing = false;
	AENGINE_API Framebuffer* Add(std::string name, Shader* frameshader, const std::vector<Framebuffer::TypeFrameBuffer>& typeFrameBuffer = { Framebuffer::TypeFrameBuffer::LDR }, bool resizable = true, int width = -1, int height = -1, const std::vector<Framebuffer::TypeFilter>& typeFilter = { Framebuffer::TypeFilter::Liner });
	AENGINE_API void Resize(int width, int height);
	AENGINE_API void Delete(std::string name);
	AENGINE_API Framebuffer* GetFramebuffer(std::string name);
	AENGINE_API std::vector<Framebuffer*> GetFramebuffers();

	AENGINE_API void Awake() override;
	AENGINE_API void OnInspector() override;
	void RecultivateProjection(); 
	AENGINE_API void SetViewport(int width, int height);
	AENGINE_API void Update() override;
	AENGINE_API ~Camera();
private:
	std::unordered_map<std::string, std::unique_ptr<Framebuffer>> framebuffers;

};