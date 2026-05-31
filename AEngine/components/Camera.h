#pragma once
#include "AComponent.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_inverse.hpp>
#include <memory>
#include "../components/Transform.h"

class Material;

class Camera : public AComponent
{
	RegisterComponent(Camera)
public:
	float fieldOView = 60.0f;
	Transform* transform;
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	float near = 0.1f;
	float far = 100.0f;

	void Start() override;

	void Update() override;
};