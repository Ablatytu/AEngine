#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <memory>
#include "../components/Transform.h"
#include "../Renderer/GameObject.h"
#include "../components/AComponent.h"
#include <string>
#include "../components/Cube.h"
#include "../components/Light.h"
#include "../core/ComponentSystem.h"
#include "../components/Rei.h"
#include "../components/Render.h"
#include "../Renderer/Mesh.h"
#include "../scene/AssetManager.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../APIs/ParserGLSL.h"
#include "../Renderer/Cubemap.h"
#include "../Renderer/RenderSystem.h"
#include <ImGuizmo.h>

Transform* transform;
float pos[3];
float scale[3];
float rot[3];
Shader* OutlineShader;
Material OutlineMaterial;
void EditorGUI::RenderOutline(Render* mesh)
{
	if (!OutlineShader)
	{
		OutlineShader = AssetManager::LoadShader("Shaders/VertexSelectedObjectEditor.glsl", "Shaders/selectedObjectEditor.glsl");
		OutlineMaterial.SetShader(OutlineShader);
	}
	RenderSystem::rs_RenderOutline(selectedObject.selectedObjectRender, OutlineShader);
}
void EditorGUI::RenderGizmo()
{
	if (!selectedObject.selectedObject)
		return;
	Transform* transform = selectedObject.selectedObject->GetComponent<Transform>();
	glm::mat4 view = SceneManager::Get().GetActiveScene().activeCamera->viewMatrix;
	glm::mat4 proj = SceneManager::Get().GetActiveScene().activeCamera->projection;

	glm::mat4 modelMatrix = transform->GetModelMatrix();

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

	ImGuiIO& io = ImGui::GetIO();
	static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_W))
		operation = ImGuizmo::TRANSLATE;

	if (ImGui::IsKeyPressed(ImGuiKey_E))
		operation = ImGuizmo::ROTATE;

	if (ImGui::IsKeyPressed(ImGuiKey_R))
		operation = ImGuizmo::SCALE;
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), operation, ImGuizmo::WORLD, glm::value_ptr(modelMatrix));
	if (ImGuizmo::IsUsing())
	{
		float pos[3], rot[3], scale[3];

		ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), pos, rot, scale);

		transform->position = glm::vec3(pos[0], pos[1], pos[2]);
		transform->rotation = Quaternion::Euler(rot[0], rot[1], rot[2]);
		transform->scale = glm::vec3(scale[0], scale[1], scale[2]);

		transform->UpdateMatrices();
	}
}
void EditorGUI::RenderEditorParts()
{
	static GameObject* prevObjOutline = nullptr;
	static Render* render = nullptr;
	static Transform* transform = nullptr;
	if (selectedObject.selectedObject)
	{
		if (selectedObject.selectedObjectRender != render)
		{
			GameObject* SOR = selectedObject.selectedObject;
			render = selectedObject.selectedObjectRender;
			transform = SOR->GetComponent<Transform>();
			prevObjOutline = SOR;
		}
		if (render && transform && render->mesh)
		{
			RenderOutline(render);
		}
	}
	else
	{
		if (render)
		{
			render = nullptr;
			transform = nullptr;
		}
		prevObjOutline = nullptr;
	}
}
void EditorGUI::ShowHierarchyWindow()
{
	ImGuiIO& io = ImGui::GetIO();

	float hierarchyWidth = 300.0f;
	float hierarchyHeight = io.DisplaySize.y;
	float hierarchyX = 0.0f;
	float hierarchyY = 0.0f;

	ImGui::SetNextWindowPos(ImVec2(hierarchyX, hierarchyY));
	ImGui::SetNextWindowSize(ImVec2(hierarchyWidth, hierarchyHeight));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Hierarchy", nullptr, flags);
	for (std::unique_ptr<GameObject>& object : SceneManager::Get().GetActiveScene().objects)
	{
		bool selected = (selectedObject.selectedObject == object.get());
		if (ImGui::Selectable((std::string(object->name) + "##" + std::to_string((uintptr_t)object.get())).c_str(), selected))
		{
			selectedObject.selectedObject = object.get();
			selectedObject.selectedObjectRender = object.get()->GetComponent<Render>();
		}
	}
	ImGui::End();
}
void EditorGUI::ShowInspectorWindow()
{
	ImGuiIO& io = ImGui::GetIO();

	float inspectorWidth = 300.0f;
	float inspectorHeight = io.DisplaySize.y;
	float inspectorX = io.DisplaySize.x - inspectorWidth;
	float inspectorY = 0;

	ImGui::SetNextWindowPos(ImVec2(inspectorX, inspectorY));
	ImGui::SetNextWindowSize(ImVec2(inspectorWidth, inspectorHeight));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	static GameObject* prevObj = nullptr;
	ImGui::Begin("Inspector", nullptr, flags);
	if (selectedObject.selectedObject)
	{
		glm::vec3 euler = glm::vec3(1.0f);
		if (prevObj != selectedObject.selectedObject)
		{
			prevObj = selectedObject.selectedObject;
			transform = selectedObject.selectedObject->GetComponent<Transform>();
			pos[0] = transform->position.x;
			pos[1] = transform->position.y;
			pos[2] = transform->position.z;
			scale[0] = transform->scale.x;
			scale[1] = transform->scale.y;
			scale[2] = transform->scale.z;
			euler = glm::degrees(glm::eulerAngles(transform->rotation));
			rot[0] = euler.x;
			rot[1] = euler.y;
			rot[2] = euler.z;
		}
		ImGui::Text(("Object name: " + std::string(selectedObject.selectedObject->name)).c_str());
		ImGui::Separator();
		if (ImGui::DragFloat3("position", pos, 0.1f))
				transform->position = glm::vec3(pos[0], pos[1], pos[2]);
		pos[0] = transform->position.x;
		pos[1] = transform->position.y;
		pos[2] = transform->position.z;
		if (ImGui::DragFloat3("rotation", rot, 0.1f))
		{
			transform->rotation = Quaternion::Euler(rot[0], rot[1], rot[2]);
		}
		euler = glm::degrees(glm::eulerAngles(transform->rotation));
		rot[0] = euler.x;
		rot[1] = euler.y;
		rot[2] = euler.z;
		if (ImGui::DragFloat3("scale", scale, 0.1f))
				transform->scale = glm::vec3(scale[0], scale[1], scale[2]);
		scale[0] = transform->scale.x;
		scale[1] = transform->scale.y;
		scale[2] = transform->scale.z;
		AComponent* removedComponent = nullptr;
		for (auto& component : selectedObject.selectedObject->components)
		{
			component->fields.clear();
			component->OnInspector();
			std::string componentName = std::string(component->GetComponentName()) + ": ";
			ImGui::Text(componentName.c_str());
			if (ImGui::BeginPopupContextItem(("ComponentContext:##" + std::to_string((uintptr_t)component.get())).c_str()))
			{
				if (!dynamic_cast<Transform*>(component.get()))
				{
					if (ImGui::MenuItem("RemoveComponent"))
					{
						removedComponent = component.get();
					}
				}
				ImGui::MenuItem("Reset");
				ImGui::MenuItem("Duplicate");

				ImGui::EndPopup();
			}
			for (auto& field : component->fields)
				switch (field.type)
				{
				case FieldType::BOOL:
					ImGui::Checkbox(field.name, (bool*)field.ptr);
					break;
				case FieldType::FLOAT:
					if (field.maxValue > field.minValue)
						ImGui::SliderFloat(field.name, (float*)field.ptr, field.minValue, field.maxValue);
					else
						ImGui::DragFloat(field.name, (float*)field.ptr, 0.1f);
					break;
				case FieldType::INT:
					if ((int)field.maxValue > (int)field.minValue)
						ImGui::SliderInt(field.name, (int*)field.ptr, (int)field.minValue, (int)field.maxValue);
					else
						ImGui::DragInt(field.name, (int*)field.ptr, 1.0f);
					break;
				}
		}
		if (ImGui::BeginCombo("AddComponent", "Select"))
		{
			if (ImGui::Selectable("Cube"))
			{
				selectedObject.selectedObject->AddComponent<Cube>();
			}
			if (ImGui::Selectable("Light"))
			{
				selectedObject.selectedObject->AddComponent<Light>();
			}
			if (ImGui::Selectable("Rei"))
			{
				selectedObject.selectedObject->AddComponent<Rei>();
			}
			ImGui::EndCombo();
		}
		transform->UpdateMatrices();
		if (removedComponent)
		{
			selectedObject.selectedObject->RemoveComponent(removedComponent);
			removedComponent = nullptr;
		}
		Render* renderComponent = selectedObject.selectedObject->GetComponent<Render>();
		if (renderComponent)
		{
			Material* material = renderComponent->material;
			if (material)
			{
				for (auto& fieldmaterial : renderComponent->material->GetProperties()->propertiesNames)
				{
					auto fieldmaterialinfo = material->GetProperties()->infoes.find(fieldmaterial);
					if (fieldmaterialinfo == material->GetProperties()->infoes.end()) continue;
					if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::Float)
					{
						float temp = 0.0f;
						if (fieldmaterialinfo->second.info.useRange)
						{
							if (auto t = material->GetProperties()->GetValue<float>(fieldmaterial))
							{
								temp = *t;
								if (ImGui::SliderFloat(fieldmaterial.c_str(), &temp, fieldmaterialinfo->second.info.minrange, fieldmaterialinfo->second.info.maxrange))
								{
									material->GetProperties()->SetValue<float>(fieldmaterial, temp);
								}
							}
						}
						else
						{
							if (auto t = material->GetProperties()->GetValue<float>(fieldmaterial))
							{
								temp = *t;
								if (ImGui::DragFloat(fieldmaterial.c_str(), &temp, 0.1f)) 
								{
									material->GetProperties()->SetValue<float>(fieldmaterial, temp);
								}
							}
						}
					}
					else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::Int)
					{
						int temp = 0.0f;
						if (fieldmaterialinfo->second.info.useRange)
						{
							if (auto t = material->GetProperties()->GetValue<int>(fieldmaterial))
							{
								temp = *t;
								if (ImGui::SliderInt(fieldmaterial.c_str(), &temp, static_cast<int>(fieldmaterialinfo->second.info.minrange), static_cast<int>(fieldmaterialinfo->second.info.maxrange)))
								{
									material->GetProperties()->SetValue<int>(fieldmaterial, temp);
								}
							}
						}
						else
						{
							if (auto t = material->GetProperties()->GetValue<int>(fieldmaterial))
							{
								temp = *t;
								if (ImGui::DragInt(fieldmaterial.c_str(), &temp, 1))
								{
									material->GetProperties()->SetValue<int>(fieldmaterial, temp);
								}
							}
						}
					}
					else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::Bool)
					{
						bool temp = false;
						if (auto t = material->GetProperties()->GetValue<bool>(fieldmaterial))
						{
							temp = *t;
							if (ImGui::Checkbox(fieldmaterial.c_str(), &temp))
							{
								material->GetProperties()->SetValue<bool>(fieldmaterial, temp);
							}
						}
					}
					else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::Vector4)
					{
						float color[4];
						if (auto t = material->GetProperties()->GetValue<GLSLParserMath::Vector4>(fieldmaterial))
						{
							color[0] = t->data[0];
							color[1] = t->data[1];
							color[2] = t->data[2];
							color[3] = t->data[3];
							if (fieldmaterialinfo->second.info.isColor)
							{
								if (fieldmaterialinfo->second.info.RGB)
								{
									if (ImGui::ColorEdit3(fieldmaterial.c_str(), color))
									{
										material->GetProperties()->SetValue<GLSLParserMath::Vector4>(fieldmaterial, GLSLParserMath::Vector4::float4_to_vector4(color));
									}
								}
								else
								{
									if (ImGui::ColorEdit4(fieldmaterial.c_str(), color))
									{
										material->GetProperties()->SetValue<GLSLParserMath::Vector4>(fieldmaterial, GLSLParserMath::Vector4::float4_to_vector4(color));
									}
								}
							}
							else
							{
								if (ImGui::DragFloat4(fieldmaterial.c_str(), color, 0.1f))
								{
									material->GetProperties()->SetValue<GLSLParserMath::Vector4>(fieldmaterial, GLSLParserMath::Vector4::float4_to_vector4(color));
								}
							}
						}
					}
					else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::Vector3)
					{
						float color[3];
						if (auto t = material->GetProperties()->GetValue<GLSLParserMath::Vector3>(fieldmaterial))
						{
							color[0] = t->data[0];
							color[1] = t->data[1];
							color[2] = t->data[2];
							if (ImGui::DragFloat3(fieldmaterial.c_str(), color, 0.1f))
							{
								material->GetProperties()->SetValue<GLSLParserMath::Vector3>(fieldmaterial, GLSLParserMath::Vector3::float3_to_vector3(color));
							}
						}
					}
					else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::Vector2)
					{
						float color[2];
						if (auto t = material->GetProperties()->GetValue<GLSLParserMath::Vector2>(fieldmaterial))
						{
							color[0] = t->data[0];
							color[1] = t->data[1];
							if (ImGui::DragFloat2(fieldmaterial.c_str(), color, 0.1f))
							{
								material->GetProperties()->SetValue<GLSLParserMath::Vector2>(fieldmaterial, GLSLParserMath::Vector2::float2_to_vector2(color));
							}
						}
					}
					else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::Sampler2D)
					{
						std::string comboId = "Textures##" + fieldmaterial;
						static std::string nameTexture = "Select";
						if (ImGui::BeginCombo(comboId.c_str(), nameTexture.c_str()))
						{
							if (ImGui::Selectable("None"))
							{
								material->Set(fieldmaterial, (Texture*)nullptr);
							}
							for (auto& texture : AssetManager::GetTextures())
							{
								if (ImGui::Selectable(texture.first.c_str()))
								{
									material->Set(fieldmaterial, texture.second.get());
								}
							}
							ImGui::EndCombo();
						}
					}
					else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::SamplerCube)
					{
						std::string comboId = "Cubemaps##" + fieldmaterial;
						if (ImGui::BeginCombo(comboId.c_str(), "Select"))
						{
							for (auto& texture : AssetManager::GetCubemaps())
							{
								if (ImGui::Selectable(texture.first.c_str()))
								{
									material->Set(fieldmaterial, texture.second.get());
								}
							}
							ImGui::EndCombo();
						}
					}
				}
			}
		}
	}
	else
	{
		transform = nullptr;
		prevObj = nullptr;
	}
	ImGui::End();
}