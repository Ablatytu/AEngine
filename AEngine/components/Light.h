#pragma once
#include "AComponent.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <memory>
class Transform;
class Material;
class Texture;
class Light : public AComponent
{
	RegisterComponent(Light)
public:
	enum class Type
	{
		Directional,
		Point,
		Spot
	};
	struct ShadowMap
	{
		unsigned int ID = 0;
		unsigned int resolution = 1024;
		unsigned int created = false;
		unsigned int IDfbo = 0;
		unsigned int VAO = 0;
		float far_plane = 25.0f;
	};
	ShadowMap shadowMap;
	bool shadow = false;
	Type typeLight = Type::Point;
	Transform* transform;
	glm::vec3 lightColor = glm::vec3(1.0f);
	int ShadowMapResolution = 1024;
	float intensive = 1.0f;
	float constant = 1.0f;
	float linear = 0.35f;
	float quadratic = 0.44f;
	float cutOff = glm::cos(glm::radians(12.5f));
	float outerCutOff = glm::cos(glm::radians(17.5f));
	float lightMax = std::fmaxf(std::fmaxf(lightColor.r, lightColor.g), lightColor.b);
	float radius =
		(-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax)))
		/ (2 * quadratic);
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0);
	void Awake() override;
	void OnInspector() override;
	void OnDestroy() override;
};