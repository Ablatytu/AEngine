#pragma once
#include "AEngineComponent.h"
#include "../core/AEngineAPI.h"

class Material;
class Texture;
class Transform;

class AENGINE_API SpriteRenderer2D : public AEngineComponent
{
	AE_REGISTER_CLASS(SpriteRenderer2D);
public:
	enum class RenderSpace
	{
		World,
		Canvas
	};
	RenderSpace renderSpace = RenderSpace::World;

	glm::vec3 color = glm::vec3(1.0f);
	Texture* texture = nullptr;
	Transform* transform = nullptr;
	Material* material = nullptr;
	void Start() override;
	void OnInspector() override;
};