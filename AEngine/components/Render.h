#ifndef RENDER_H
#define RENDER_H
#include "AEngineComponent.h"
#include <memory>
#include "../Renderer/Material.h"
#include "Transform.h"
#include "../core/AEngineAPI.h"

class Mesh;
class Shader;

enum RenderLayer
{
	defaultRender = 0,
	stencilTest = 1
};
class AENGINE_API Render : public AEngineComponent
{
	AE_REGISTER_CLASS(Render);
public:
	RenderLayer renderLayer = RenderLayer::defaultRender;
	Material* material = nullptr;
	Mesh* mesh;
	Transform* transform;
	void Start() override;
	void OnInspector() override;
};

#endif