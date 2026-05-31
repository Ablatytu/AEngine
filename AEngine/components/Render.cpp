#include "Render.h"
#include "../Renderer/Mesh.h"   
#include "../Renderer/Shader.h"
#include "../Renderer/GameObject.h"

void Render::Start()
{
	transform = gameObject->GetComponent<Transform>();
}
