#include "../Renderer/GameObject.h"	
#include "cube.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

void Cube::Start()
{
	transform = gameObject->GetComponent<Transform>();
}													
void Cube::Update()
{
	if (rotate)
	{
		transform->rotation = Quaternion::Euler((float)glfwGetTime() * 25.0f, (float)glfwGetTime() * 50.0f, (float)glfwGetTime() * 10.0f);
		transform->UpdateMatrices();
	}
}
void Cube::OnInspector()
{
	RegisterField(rotate)
}

AE_REGISTER_COMPONENT(Cube)