#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "EditorCamera.h"
#include "../AEngine/components/Render.h"
#include "../AEngine/Renderer/GameObject.h"
#include "../AEngine/core/Window.h"
#include "../AEngine/Renderer/Mesh.h"
#include "Editor.h"
#include <AMath.h>
#include "../AEngine/core/Application.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <ImGuizmo.h>

glm::vec3 EditorCamera::CalcMouseRay()
{
	float mouseX = Input::GetMouseX();
	float mouseY = Input::GetMouseY();

	float width = (float)editorGUI->sizeviewport[0];
	float height = (float)editorGUI->sizeviewport[1];

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
EditorCamera::EditorCamera(GameObject* _cameraObject, EditorGUI* _editorGUI)
{
	cameraObject = _cameraObject;
	editorGUI = _editorGUI;
	transform = cameraObject->GetComponent<Transform>();
	camera = cameraObject->GetComponent<Camera>();
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
		if (editorGUI->viewportHovered && !(ImGuizmo::IsUsing() || ImGuizmo::IsOver()))
		{
			GameObject* selectedObject = FindClosestObject();
			editorGUI->selectedObject.selectedObject = selectedObject;
			if(selectedObject)
				editorGUI->selectedObject.selectedObjectRender = selectedObject->GetComponent<Render>();
			else
				editorGUI->selectedObject.selectedObjectRender = nullptr;
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