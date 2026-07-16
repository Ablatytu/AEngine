#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <memory>
#include "../AEngine/components/Transform.h"
#include "../AEngine/Renderer/GameObject.h"
#include "../AEngine/components/AComponent.h"
#include <string>
#include "../AEngine/components/Cube.h"
#include "../AEngine/components/Light.h"
#include "../AEngine/core/ComponentSystem.h"
#include "../AEngine/components/Rei.h"
#include "../AEngine/components/Render.h"
#include "../AEngine/Renderer/Mesh.h"
#include "../AEngine/scene/AssetManager.h"
#include "../AEngine/Renderer/Shader.h"
#include "../AEngine/Renderer/Texture.h"
#include "../AEngine/APIs/ParserGLSL.h"
#include "../AEngine/Renderer/Cubemap.h"
#include "../AEngine/Renderer/RenderSystem.h"
#include "../AEngine/Renderer/FramebufferManager.h"
#include "../AEngine/Renderer/Framebuffer.h"
#include "imgui_internal.h"
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
void EditorGUI::EDITOR_INIT()
{
	sceneHDR = FramebufferManager::Get().Add("defaultFramebuffer", "Shaders/FramebufferVertexShader.glsl", "Shaders/FramebufferFragmentShader.glsl", { Framebuffer::TypeFrameBuffer::HDR }, true);
	viewport = FramebufferManager::Get().Add("ViewportFramebuffer", "__NULL__", "__NULL__", { Framebuffer::TypeFrameBuffer::HDR }, true);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
void EditorGUI::RenderViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");
	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	viewportBounds[0][0] = viewportMinRegion.x + viewportOffset.x;
	viewportBounds[0][1] = viewportMinRegion.y + viewportOffset.y;

	viewportBounds[1][0] = viewportMaxRegion.x + viewportOffset.x;
	viewportBounds[1][1] = viewportMaxRegion.y + viewportOffset.y;

	float sizeviewportWidth = viewportBounds[1][0] - viewportBounds[0][0];
	float sizeviewportHeight = viewportBounds[1][1] - viewportBounds[0][1];
	sizeviewport[0] = std::max(1, (int)sizeviewportWidth);
	sizeviewport[1] = std::max(1, (int)sizeviewportHeight);
	viewportHovered = ImGui::IsWindowHovered();
	ImGui::Image((ImTextureID)(uintptr_t)viewport->GetColorBuffer(0), ImVec2(sizeviewportWidth, sizeviewportHeight), ImVec2(0, 1), ImVec2(1, 0)	);
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(viewportBounds[0][0], viewportBounds[0][1], sizeviewportWidth, sizeviewportHeight);
	RenderGizmo();
	ImGui::End();
	ImGui::PopStyleVar();
}
void EditorGUI::BeginDockspace()
{
	static bool dockspaceOpen = true;

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("MainDockspaceWindow", &dockspaceOpen, windowFlags);

	ImGui::PopStyleVar(2);

	ImGuiID dockspaceID = ImGui::GetID("MainDockspace");

	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	static bool firstTime = true;
	if (firstTime)
	{
		firstTime = false;

		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

		ImGuiID dockMain = dockspaceID;
		ImGuiID dockLeft = 0;
		ImGuiID dockRight = 0;

		ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.22f, &dockLeft, &dockMain);
		ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.28f, &dockRight, &dockMain);

		ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
		ImGui::DockBuilderDockWindow("Inspector", dockRight);
		ImGui::DockBuilderDockWindow("Viewport", dockMain);

		ImGui::DockBuilderFinish(dockspaceID);
	}

	ImGui::End();
}
void EditorGUI::RenderGizmo()
{
	if (!selectedObject.selectedObject)
		return;
	Transform* transform = selectedObject.selectedObject->GetComponent<Transform>();
	glm::mat4 view = SceneManager::Get().GetActiveScene().activeCamera->viewMatrix;
	glm::mat4 proj = SceneManager::Get().GetActiveScene().activeCamera->projection;

	glm::mat4 modelMatrix = transform->GetModelMatrix();

	static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_W))
		operation = ImGuizmo::TRANSLATE;

	if (ImGui::IsKeyPressed(ImGuiKey_E))
		operation = ImGuizmo::ROTATE;

	if (ImGui::IsKeyPressed(ImGuiKey_R))
		operation = ImGuizmo::SCALE;
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

	ImGui::Begin("Hierarchy");
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

	static GameObject* prevObj = nullptr;
	ImGui::Begin("Inspector");
	if (selectedObject.selectedObject)
	{
		AComponent* removedComponent = nullptr;
		for (auto& component : selectedObject.selectedObject->components)
		{
			component->fields.clear();
			component->OnInspector();
			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_FramePadding;
			bool opened = ImGui::TreeNodeEx(component.get(), flags, "%s", component->GetComponentName());
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
			if (opened)
			{
				for (auto& field : component->fields)
				{
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
						case FieldType::VEC3:
						{
							auto* value3 = static_cast<glm::vec3*>(field.ptr);
							ImGui::DragFloat3(field.name, glm::value_ptr(*value3), 0.1f);
							break;
						}
						case FieldType::QUAT:
						{
							auto* valueq = static_cast<glm::quat*>(field.ptr);
							glm::vec3 euler = glm::degrees(glm::eulerAngles(*valueq));
							float data[3] = { euler.x, euler.y,euler.z };
							if (ImGui::DragFloat3(field.name, data, 0.1f))
							{
								*valueq = Quaternion::Euler(data[0], data[1], data[2]);
							}
							break;
						}
					}

				}
				ImGui::TreePop();
			}
		}
		if (auto* selectedTransform = selectedObject.selectedObject->GetComponent<Transform>())
		{
			selectedTransform->UpdateMatrices();
		}
		if (removedComponent)
		{
			selectedObject.selectedObject->RemoveComponent(removedComponent);
			removedComponent = nullptr;
		}
		Render* renderComponent = selectedObject.selectedObject->GetComponent<Render>();
		if (renderComponent)
		{
			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_FramePadding;
			Material* material = renderComponent->material;
			if (material)
			{
				bool opened = ImGui::TreeNodeEx(material, flags, "%s", "some material");
				if (opened)
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
							std::string comboId = fieldmaterial;
							Texture* selectedTexture = material->GetTexture(fieldmaterial);
							const char* preview = selectedTexture ? selectedTexture->path.c_str() : "None";
							if (ImGui::BeginCombo(comboId.c_str(), preview))
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
							std::string comboId = fieldmaterial;
							std::string nameTexture = std::get<std::string>(fieldmaterialinfo->second.info.value);
							if (ImGui::BeginCombo(comboId.c_str(), nameTexture.c_str()))
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
					ImGui::TreePop();
				}
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
	}
	else
	{
		transform = nullptr;
		prevObj = nullptr;
	}
	ImGui::End();
}