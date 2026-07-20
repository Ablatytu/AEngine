#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <memory>
#include <filesystem>
#include <algorithm>
#include <cctype>
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
#include "../AEngine/APIs/ModelLoader.h"
#include "../AEngine/core/Application.h"
#include "imgui_internal.h"
#include <ImGuizmo.h>

void EditorGUI::RenderOutline(Render* mesh)
{
	if (!OutlineShader)
	{
		OutlineShader = AssetManager::LoadShader("Shaders/VertexSelectedObjectEditor.glsl", "Shaders/selectedObjectEditor.glsl");
	}
	RenderSystem::rs_RenderOutline(selectedObject.selectedObjectRender, OutlineShader);
}
void EditorGUI::EDITOR_INIT()
{
	sceneHDR = FramebufferManager::Get().Add("defaultFramebuffer", "Shaders/FramebufferVertexShader.glsl", "Shaders/FramebufferFragmentShader.glsl", { Framebuffer::TypeFrameBuffer::HDR }, true);
	viewport = FramebufferManager::Get().Add("ViewportFramebuffer", "__NULL__", "__NULL__", { Framebuffer::TypeFrameBuffer::HDR }, true);
	folderIcon = std::make_unique<Texture>();
	folderIcon->LoadTexture("EditorAssets/FolderIconAEngine.png");
	ModelIcon = std::make_unique<Texture>();
	ModelIcon->LoadTexture("EditorAssets/3DModelIconAEngine.png");
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
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET"))
		{
			const char* relativePath = static_cast<const char*>(payload->Data);
			std::filesystem::path fullPath = projectDirectory / relativePath;
			std::string extension = fullPath.extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char symbol) {return static_cast<char>(std::tolower(symbol)); });
			if (extension == ".obj")
			{
				Model* model = AssetManager::LoadModel(fullPath.string());
				if (model)
				{
					GameObject* newObject = SceneManager::Get().GetActiveScene().CreateGameObject();
					newObject->name = fullPath.stem().string();
					Render* render = newObject->AddComponent<Render>();

					render->mesh = &model->meshes[0];
					Material* material = AssetManager::CreateMaterial(AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShaderDeferredGeometryTest.glsl"));

					render->material = material;
					selectedTexture = nullptr;

					selectedObject.selectedObject = newObject;
					selectedObject.selectedObjectRender = render;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(viewportBounds[0][0], viewportBounds[0][1], sizeviewportWidth, sizeviewportHeight);
	RenderGizmo();
	ImGui::End();
	ImGui::PopStyleVar();
}
void EditorGUI::DrawDockspaceMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene(Not working now)", "Ctrl+N"), false, false)
			{

			}
			if (ImGui::MenuItem("Save Scene(Not working now)", "Ctrl+S"), false, false)
			{

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
				Application::Get().close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				GameObject* newObject = SceneManager::Get().CreateGameObject();
				selectedObject.selectedObject = newObject;
			}
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{

				}
				if (ImGui::MenuItem("Point Light"))
				{

				}
				if (ImGui::MenuItem("Spot Light"))
				{

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::BeginMenu("Rendering"))
			{
				ImGui::MenuItem("Lighting", nullptr, &showLightingMenu);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}
void EditorGUI::ShowLightingMenu()
{
	if (!showLightingMenu)
		return;
	ImGui::SetNextWindowSize(ImVec2(400.0f, 300.0f), ImGuiCond_FirstUseEver );
	if (ImGui::Begin("Lighting", &showLightingMenu))
	{
		ImGui::TextUnformatted("Environment Lighting");

		ImGui::Button("Drop skybox", ImVec2(ImGui::GetContentRegionAvail().x, 60.0f));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET"))
			{
				const char* relativePath = static_cast<const char*>(payload->Data);

				std::filesystem::path fullpath = projectDirectory / relativePath;

				std::string extension = fullpath.extension().string();

				std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char symbol) { return static_cast<char>(std::tolower(symbol)); });

				if (extension == ".hdr")
				{
					HDRTexture* newTexture = AssetManager::LoadHDRTexture(fullpath.generic_string());
					if (newTexture)
					{
						SceneManager::Get().GetActiveScene().SetSkyBox(newTexture);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}
void EditorGUI::ShowProject()
{
	ImGui::Begin("Project");
	if (!std::filesystem::exists(projectDirectory))
		std::filesystem::create_directories(projectDirectory);
	if (currentDirectory != projectDirectory)
	{
		if (ImGui::Button("<-"))
			currentDirectory = currentDirectory.parent_path();
		ImGui::SameLine();
	}
	ImGui::TextUnformatted(currentDirectory.string().c_str());
	ImGui::Separator();
	
	std::error_code error;
	for (const auto& entry : std::filesystem::directory_iterator(currentDirectory, error))
	{
		const std::filesystem::path& path = entry.path();
		std::string filename = path.filename().string();

		ImGui::PushID(path.string().c_str());

		if (entry.is_directory())
		{
			float iconSize = ImGui::GetTextLineHeight();
			ImGui::Image((ImTextureID)(uintptr_t)folderIcon->GetTexturePixels(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
			ImGui::SameLine();
			if (ImGui::Selectable(filename.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					currentDirectory = path;
				}
			}
		}
		else
		{
			std::string extension = path.extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char symbol) { return static_cast<char>(std::tolower(symbol));});
			float iconSize = ImGui::GetTextLineHeight();
			if (extension == ".obj")
			{
				ImGui::Image((ImTextureID)(uintptr_t)ModelIcon->GetTexturePixels(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
				ImGui::SameLine();
			}
			else if (extension == ".png" || extension == ".jpg")
			{
				Texture* previewTexture = AssetManager::LoadTexture(path.generic_string());
				if (previewTexture)
				{
					ImGui::Image((ImTextureID)(uintptr_t)previewTexture->GetTexturePixels(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
					ImGui::SameLine();
				}

			}
			bool clicked = ImGui::Selectable(filename.c_str());
			if (clicked)
			{
				if (extension == ".png" || extension == ".jpg")
				{
					selectedTexture = AssetManager::LoadTexture(path.generic_string());
					selectedObject.selectedObject = nullptr;
					selectedObject.selectedObjectRender = nullptr;
				}
				else
					selectedTexture = nullptr;
			}
			if (ImGui::BeginDragDropSource())
			{
				std::string relativePath = std::filesystem::relative(path, projectDirectory).generic_string();
				ImGui::SetDragDropPayload("PROJECT_ASSET", relativePath.c_str(), relativePath.size() + 1);
				ImGui::TextUnformatted(filename.c_str());
				ImGui::EndDragDropSource();
			}
		}
		ImGui::PopID();
	}
	if (error)
	{
		ImGui::Text("Directory error: %s", error.message().c_str());
	}
	ImGui::End();
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
		ImGuiID dockDown = 0;

		ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.22f, &dockLeft, &dockMain);
		ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.28f, &dockRight, &dockMain);
		ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.22f, &dockDown, &dockMain);

		ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
		ImGui::DockBuilderDockWindow("Inspector", dockRight);
		ImGui::DockBuilderDockWindow("Viewport", dockMain);
		ImGui::DockBuilderDockWindow("Project", dockDown);

		ImGui::DockBuilderFinish(dockspaceID);
	}
	DrawDockspaceMenuBar();
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
			selectedTexture = nullptr;

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
	if (selectedTexture)
	{
		ImGui::TextUnformatted(selectedTexture->path.c_str());
		ImGui::Separator();

		float avaliableWidth = ImGui::GetContentRegionAvail().x;
		float previewWidth = std::min(avaliableWidth, 256.0f);

		float aspect = static_cast<float>(selectedTexture->GetWidth()) / static_cast<float>(selectedTexture->GetHeight());

		ImVec2 previewSize;

		if (aspect >= 1.0f)
		{
			previewSize.x = previewWidth;
			previewSize.y = previewWidth / aspect;
		}
		else
		{
			previewSize.y = previewWidth;
			previewSize.x = previewWidth * aspect;
		}

		ImGui::Image((ImTextureID)(uintptr_t)selectedTexture->GetTexturePixels(), previewSize, ImVec2(0, 0), ImVec2(1, 1));

		ImGui::Text("%d x %d", selectedTexture->GetWidth(), selectedTexture->GetHeight());
		bool linear = selectedTexture->GetLinear();
		if (ImGui::Checkbox("Linear: ", &linear))
			selectedTexture->SetLinear(linear);
		ImGui::End();
		return;
	}
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
							ImGui::BeginGroup();
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
							ImGui::EndGroup();

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET"))
								{
									const char* relativePath = static_cast<const char*>(payload->Data);
									std::filesystem::path fullpath = projectDirectory / relativePath;
									std::string extension = fullpath.extension().string();

									std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {return static_cast<char>(std::tolower(c)); });

									if (extension == ".png" || extension == ".jpg")
									{
										Texture* newTexture = AssetManager::LoadTexture(fullpath.generic_string());
										if (newTexture)
											material->Set(fieldmaterial, newTexture);
									}
								}
								ImGui::EndDragDropTarget();
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