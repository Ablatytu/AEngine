#include "Render.h"
#include "../Renderer/Mesh.h"   
#include "../Renderer/Shader.h"
#include "../Renderer/GameObject.h"

void Render::Start()
{
	transform = gameObject->GetComponent<Transform>();
}
void Render::OnInspector()
{
	RegisterField(material);
	RegisterField(mesh);
}