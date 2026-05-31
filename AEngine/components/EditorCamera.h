#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "AComponent.h"
#include <memory>
class Transform;
class Camera;
class EditorCamera : public AComponent
{
	RegisterComponent(EditorCamera)
public:
	Transform* transform;
	Camera* camera;
	glm::vec3 direction;
	float sens = 0.2f;
	float pitch, yaw = 0.0f;
	void Start() override;
	void Update() override;
	glm::vec3 CalcMouseRay();
	GameObject* FindClosestObject();
};