#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "EditorCamera.h"
#include "Render.h"
#include "../Renderer/GameObject.h"
#include "../core/Window.h"
#include "../Renderer/Mesh.h"
#include <AMath.h>
#include "../core/Application.h"
#include <ImGuizmo.h>

void EditorCamera::Start()
{
	transform = gameObject->GetComponent<Transform>();
	camera = gameObject->GetComponent<Camera>();
}
glm::vec3 EditorCamera::CalcMouseRay()
{
	float mouseX = (float)Input::xpos;
	float mouseY = (float)Input::ypos;

	float width = Window::GlobalWindow->screenX;
	float height = Window::GlobalWindow->screenY;

	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	glm::vec2 rayNDC = glm::vec2(x, y);


	glm::mat4 proj = camera->projection;
	glm::mat4 viewMatrix = camera->viewMatrix;

	glm::vec4 rayClip(rayNDC.x, rayNDC.y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(proj) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	rayEye.z = -1.0f;
	rayEye.w = 0.0f;
	glm::mat4 invView = glm::inverse(viewMatrix);
	glm::vec3 rayWorld = glm::normalize(glm::vec3(invView * rayEye));

	return rayWorld;
}
GameObject* EditorCamera::FindClosestObject()
{
	float closest = FLT_MAX;
	GameObject* selected = nullptr;
	glm::vec3 rayDir = CalcMouseRay();
	glm::vec3 rayOrigin = transform->position;
	for (auto& obj : SceneManager::Get().GetActiveScene().objects)
	{
		auto mr = obj->GetComponent<Render>();

		if (!mr || !mr->mesh)
			continue;
		glm::mat4 matrixModel = obj->GetComponent<Transform>()->GetModelMatrix();
		for (size_t i = 0; i < mr->mesh->indices.size(); i += 3)
		{
			glm::vec3 v0 = glm::vec3(matrixModel * glm::vec4(mr->mesh->vertices[mr->mesh->indices[i]].Position, 1.0f));
			glm::vec3 v1 = glm::vec3(matrixModel * glm::vec4(mr->mesh->vertices[mr->mesh->indices[i + 1]].Position, 1.0f));
			glm::vec3 v2 = glm::vec3(matrixModel * glm::vec4(mr->mesh->vertices[mr->mesh->indices[i + 2]].Position, 1.0f));

			float t = triangle_intersection(rayOrigin, rayDir, v0, v1, v2);
			if (t >= 0.0f)
			{
				if (t < closest)
				{
					closest = t;
					selected = obj.get();
				}
			}
		}
	}
	return selected;
}
void EditorCamera::Update()
{
	float zoomY = Input::GetAxis("Mouse ScrollWheel");
	transform->position += transform->forward() * zoomY;
	if (Input::GetMouseButton(1))
	{
		float x = Input::GetAxis("Mouse X") * sens;
		float y = Input::GetAxis("Mouse Y") * sens;

		pitch -= y;
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		yaw += x;

		transform->rotation = Quaternion::Euler(pitch, yaw, 0.0f);

		float xMove = Input::GetAxis("Horizontal");
		float yMove = Input::GetAxis("Vertical");

		glm::vec3 Move = glm::vec3(transform->right() * xMove + transform->forward() * yMove);
		transform->position += Move * 0.05f;

	}
	transform->UpdateMatrices();
	if (Input::GetMouseButtonDown(0))
	{
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse && !(ImGuizmo::IsUsing() || ImGuizmo::IsOver()))
		{
			GameObject* selectedObject = FindClosestObject();
			Window::GlobalWindow->editorGUI->selectedObject.selectedObject = selectedObject;
			if(selectedObject)
				Window::GlobalWindow->editorGUI->selectedObject.selectedObjectRender = selectedObject->GetComponent<Render>();
			else
				Window::GlobalWindow->editorGUI->selectedObject.selectedObjectRender = nullptr;
			std::cout << "click" << std::endl;
		}
	}
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		Application::Get().close();
	}
	//float newFOV = camera->fieldOView - zoomY;
	//if (newFOV > 45.0f) newFOV = 45.0f;
	//if (newFOV < 30.0f) newFOV = 30.0f;
	//camera->fieldOView = newFOV;
}