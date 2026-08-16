#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <memory>
#include "../core/AEngineAPI.h"
#include "AEngineComponent.h"
class Transform;
class Material;
class Texture;
class AENGINE_API Light : public AEngineComponent
{
	AE_REGISTER_CLASS(Light);
public:
	~Light() override;
	enum class Type
	{
		Directional = 0,
		Point = 1,
		Spot = 2
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
	float cutOff = glm::cos(glm::radians(12.5f));
	float outerCutOff = glm::cos(glm::radians(17.5f));
	float Radius = 5.0f;
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0);
	void Awake() override;
	void OnInspector() override;
	void OnDestroy() override;
};