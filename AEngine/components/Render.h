#ifndef RENDER_H
#define RENDER_H
#include "AComponent.h"
#include <memory>
#include "../Renderer/Material.h"
#include "Transform.h"

class Mesh;
class Shader;

enum RenderLayer
{
	defaultRender = 0,
	stencilTest = 1
};
class Render : public AComponent
{
	RegisterComponent(Render)
public:
	RenderLayer renderLayer = RenderLayer::defaultRender;
	Material* material;
	Mesh* mesh;
	Transform* transform;
	void Start() override;
};

#endif