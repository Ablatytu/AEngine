#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/quaternion.hpp>
#include "AComponent.h"
#include "../core/AEngineAPI.h"

class AENGINE_API Transform : public AComponent
{
	RegisterComponent(Transform)
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 GetModelMatrix();
	void UpdateMatrices();
	void OnInspector() override;
	glm::vec3 forward();
	glm::vec3 right();
	glm::vec3 up();
private:
	glm::mat4 matrixModel = glm::mat4(1.0f);
};
class Quaternion
{
public:
	AENGINE_API static glm::quat Euler(float x, float y, float z);
};