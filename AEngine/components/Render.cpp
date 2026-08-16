#include "../Renderer/GameObject.h"
#include "Render.h"
#include "../Renderer/Mesh.h"   
#include "../Renderer/Shader.h"

void Render::Start()
{
	transform = gameObject->GetComponent<Transform>();
}
void Render::OnInspector()
{
	RegisterField(material);
	RegisterField(mesh);
}

AE_REGISTER_COMPONENT(Render);