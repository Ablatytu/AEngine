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
#include "../AEngine/components/AEngineComponent.h"
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
#include "../AEngine/Renderer/Framebuffer.h"
#include "../AEngine/APIs/ModelLoader.h"
#include "../AEngine/core/Application.h"
#include "imgui_internal.h"
#include "../AEngine/scene/CubemapSerializer.h"
#include "../AEngine/scene/MaterialSerializer.h"
#include "../AEngine/scene/SceneSerializer.h"
#include "../AEngine/scene/SceneManager.h"
#include "../AEngine/scene/Scene.h"
#include "../AEngine/components/Camera.h"
#include "../AEngine/components/GameModule.h"
#include "../GameAPI/GameABI.h"
#include <ImGuizmo.h>

void EditorGUI::RenderOutline(Render* mesh)
{
	if (!OutlineShader)
	{
		OutlineShader = std::make_unique<Shader>("Shaders/VertexSelectedObjectEditor.glsl", "Shaders/selectedObjectEditor.glsl");
	}
	RenderSystem::rs_RenderOutline(selectedObject.selectedObjectRender, OutlineShader.get());
}
void EditorGUI::EDITOR_INIT()
{
	folderIcon = std::make_unique<Texture>();
	folderIcon->LoadTexture("EditorAssets/FolderIconAEngine.png");
	ModelIcon = std::make_unique<Texture>();
	ModelIcon->LoadTexture("EditorAssets/3DModelIconAEngine.png");
	CubemapIcon = std::make_unique<Texture>();
	CubemapIcon->LoadTexture("EditorAssets/CubemapIconAEngine.png");
	MaterialIcon = std::make_unique<Texture>();
	MaterialIcon->LoadTexture("EditorAssets/MaterialIconAEngine.png");
	StopIcon = std::make_unique<Texture>();
	StopIcon->LoadTexture("EditorAssets/StopIconAEngine.png");
	PlayIcon = std::make_unique<Texture>();
	PlayIcon->LoadTexture("EditorAssets/PlayIconAEngine.png");
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
void EditorGUI::RenderGameViewport(Camera* camera)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Game");
	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	gameportBounds[0][0] = viewportMinRegion.x + viewportOffset.x;
	gameportBounds[0][1] = viewportMinRegion.y + viewportOffset.y;

	gameportBounds[1][0] = viewportMaxRegion.x + viewportOffset.x;
	gameportBounds[1][1] = viewportMaxRegion.y + viewportOffset.y;

	float sizeviewportWidth = gameportBounds[1][0] - gameportBounds[0][0];
	float sizeviewportHeight = gameportBounds[1][1] - gameportBounds[0][1];
	sizegameport[0] = std::max(1, (int)sizeviewportWidth);
	sizegameport[1] = std::max(1, (int)sizeviewportHeight);

	if (camera)
		ImGui::Image((ImTextureID)(uintptr_t)camera->GetFramebuffer("ViewportFramebuffer")->GetColorBuffer(0), ImVec2(sizeviewportWidth, sizeviewportHeight), ImVec2(0, 1), ImVec2(1, 0));
	else
		ImGui::ColorButton("##Nonecamera", ImVec4(0.0f, 0.0f, 0.0f, 1.0f), 0, ImVec2(sizeviewportWidth, sizeviewportHeight));
	ImGui::End();
	ImGui::PopStyleVar();
}
void EditorGUI::RenderViewport(Camera* camera)
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
	ImGui::Image((ImTextureID)(uintptr_t)camera->GetFramebuffer("ViewportFramebuffer")->GetColorBuffer(0), ImVec2(sizeviewportWidth, sizeviewportHeight), ImVec2(0, 1), ImVec2(1, 0)	);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET"))
		{
			const char* relativePath = static_cast<const char*>(payload->Data);
			std::filesystem::path fullPath = projectDirectory / relativePath;
			std::string extension = fullPath.extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char symbol) {return static_cast<char>(std::tolower(symbol)); });
			if (extension == ".obj" || extension == ".fbx")
			{
				Model* model = AssetManager::LoadModel(fullPath.string());
				if (model)
				{
					selectedTexture = nullptr;
					selectedCubemap = nullptr;
					selectedMaterial = nullptr;
					GameObject* newObject = SceneManager::Get().GetActiveScene().CreateGameObject();
					newObject->name = fullPath.stem().string();
					if (model->meshes.size() == 1)
					{
						Material* material = AssetManager::CreateMaterial("StandartMaterial", AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShaderDeferredGeometryTest.glsl"));
						Render* render = newObject->AddComponent<Render>();
						render->mesh = &model->meshes[0];
						render->material = material;
						selectedObject.selectedObject = newObject;
						selectedObject.selectedObjectRender = render;
						selectedAsset = SelectedAsset::None;
					}
					else if(model->meshes.size() > 1)
					{
						for (auto& mesh : model->meshes)
						{
							GameObject* childObject = SceneManager::Get().GetActiveScene().CreateGameObject();
							childObject->name = fullPath.stem().string();
							Render* render = childObject->AddComponent<Render>();

							render->mesh = &mesh;
							Material* material = AssetManager::CreateMaterial("StandartMaterial", AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShaderDeferredGeometryTest.glsl"));

							childObject->GetComponent<Transform>()->SetParent(newObject->GetComponent<Transform>());
							render->material = material;
						}
						selectedObject.selectedObject = newObject;
						selectedObject.selectedObjectRender = nullptr;
						selectedAsset = SelectedAsset::None;
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(viewportBounds[0][0], viewportBounds[0][1], sizeviewportWidth, sizeviewportHeight);
	RenderGizmo(camera);
	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorGUI::SetGameMode()
{
	sceneMode = SceneMode::Play;
	pastScenePathBeforePlayMode = SceneManager::Get().GetActiveScene().pathScene;
	snapshotScene = SceneSerializer::SerializeToString(SceneManager::Get().GetActiveScene());
	Application::Get().UpdateGameModule(true);
}
void EditorGUI::SetEditMode()
{
	sceneMode = SceneMode::Edit;
	SceneManager::Get().LoadSceneFromString(snapshotScene, pastScenePathBeforePlayMode);
	pastScenePathBeforePlayMode = "";
	Application::Get().UpdateGameModule(false);
}
void EditorGUI::DrawToolbar()
{
	const float toolbarHeight = 36.0f;

	ImGui::BeginChild("##ToolbarEditor", ImVec2(0.0f, toolbarHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	const float buttonWidth = 26.0f;
	const float buttonHeight = 26.0f;

	float betweenSpace = ImGui::GetStyle().ItemSpacing.x;

	float totalWidth = buttonWidth * 2.0f + betweenSpace;

	float startX = (ImGui::GetWindowWidth() - totalWidth) * 0.5f;
	ImGui::SetCursorPosX(startX);

	if (sceneMode == SceneMode::Play)
	{
		ImGui::BeginDisabled();
		if (ImGui::ImageButton("##PlayButton", PlayIcon->GetTexturePixels(), ImVec2(buttonWidth, buttonHeight)))
		{
			//something;
		}
		ImGui::EndDisabled();
	}
	else
	{
		if (ImGui::ImageButton("##PlayButton", PlayIcon->GetTexturePixels(), ImVec2(buttonWidth, buttonHeight)))
		{
			SetGameMode();
		}
	}
	ImGui::SameLine();
	if (sceneMode == SceneMode::Play)
	{
		if (ImGui::ImageButton("##StopButton", StopIcon->GetTexturePixels(), ImVec2(buttonWidth, buttonHeight)))
		{
			SetEditMode();
		}
	}
	else
	{
		ImGui::BeginDisabled();
		if (ImGui::ImageButton("##StopButton", StopIcon->GetTexturePixels(), ImVec2(buttonWidth, buttonHeight)))
		{
			//something;
		}
		ImGui::EndDisabled();
	}

	ImGui::EndChild();
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
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
			{
				SceneSerializer::Serialize(SceneManager::Get().GetActiveScene(), SceneManager::Get().GetActiveScene().pathScene);
			}
			if (ImGui::MenuItem("Reload scripts", "Ctrl+Shift+R"))
			{
				GameModule::Reload();
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
				selectedTexture = nullptr;
				selectedCubemap = nullptr;
				selectedMaterial = nullptr;
				selectedAsset = SelectedAsset::None;
				selectedObject.selectedObject = newObject;
				selectedObject.selectedObjectRender = nullptr;
			}
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					GameObject* newObject = SceneManager::Get().CreateGameObject();
					newObject->AddComponent<Light>();
					newObject->GetComponent<Light>()->typeLight = Light::Type::Directional;
					selectedTexture = nullptr;
					selectedCubemap = nullptr;
					selectedMaterial = nullptr;
					selectedAsset = SelectedAsset::None;
					selectedObject.selectedObject = newObject;
					selectedObject.selectedObjectRender = nullptr;
				}
				if (ImGui::MenuItem("Point Light"))
				{
					GameObject* newObject = SceneManager::Get().CreateGameObject();
					newObject->AddComponent<Light>();
					newObject->GetComponent<Light>()->typeLight = Light::Type::Point;
					selectedTexture = nullptr;
					selectedCubemap = nullptr;
					selectedMaterial = nullptr;
					selectedAsset = SelectedAsset::None;
					selectedObject.selectedObject = newObject;
					selectedObject.selectedObjectRender = nullptr;

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
			if (extension == ".obj" || extension == ".fbx")
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
			else if (extension == ".acubemap")
			{
				if (AssetManager::LoadCubemap(path.generic_string()))
				{
					ImGui::Image((ImTextureID)(uintptr_t)CubemapIcon->GetTexturePixels(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
					ImGui::SameLine();
				}
			}
			else if (extension == ".amat")
			{
				if(AssetManager::LoadMaterial(path.generic_string()))
				{
					ImGui::Image((ImTextureID)(uintptr_t)MaterialIcon->GetTexturePixels(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
					ImGui::SameLine();
				}
			}
			bool clicked = ImGui::Selectable(filename.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
			if (clicked)
			{
				selectedTexture = nullptr;
				selectedCubemap = nullptr;
				selectedMaterial = nullptr;
				selectedAsset = SelectedAsset::None;
				selectedObject.selectedObject = nullptr;
				selectedObject.selectedObjectRender = nullptr;
				if (extension == ".png" || extension == ".jpg")
				{
					selectedAsset = SelectedAsset::Texture;
					selectedTexture = AssetManager::LoadTexture(path.generic_string());
				}
				else if (extension == ".acubemap")
				{
					if (CubemapSerializer::IsCubemap(path.generic_string()))
					{
						selectedCubemap = AssetManager::LoadCubemap(path.generic_string());
						selectedAsset = SelectedAsset::Cubemap;
					}
				}
				else if (extension == ".amat")
				{
					if (MaterialSerializer::IsMaterial(path.generic_string()))
					{
						selectedAsset = SelectedAsset::Material;
						selectedMaterial = AssetManager::LoadMaterial(path.generic_string());
					}
				}
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (extension == ".ascene")
				{
					SceneManager::Get().LoadScene(path.generic_string());
				}
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
	if (ImGui::BeginPopupContextWindow("ProjectContextWindow", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
	{
		if (ImGui::MenuItem("Create new cubemap"))
		{
			int index = 1;
			std::filesystem::path path = currentDirectory / "NewCubemap.acubemap";
			while (std::filesystem::exists(path))
			{
				path = currentDirectory / ("NewCubemap" + std::to_string(index) + ".acubemap");
				index++;
			}
			AssetManager::CreateCubemap(path.generic_string(), 512, true);
		}
		if (ImGui::MenuItem("Create new material"))
		{
			int index = 1;
			std::filesystem::path path = currentDirectory / "NewMaterial.amat";
			while (std::filesystem::exists(path))
			{
				path = currentDirectory / ("NewMaterial" + std::to_string(index) + ".amat");
				index++;
			}
			AssetManager::CreateMaterial(path.generic_string(), AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShaderDeferredGeometryTest.glsl"), true);
		}
		if (ImGui::MenuItem("Create new scene"))
		{
			int index = 1;
			std::filesystem::path path = currentDirectory / "NewScene.ascene";
			while (std::filesystem::exists(path)) 
			{
				path = currentDirectory / ("NewScene" + std::to_string(index) + ".ascene");
				index++;
			}
			SceneManager::Get().CreateScene(path.generic_string());
		}
		ImGui::EndPopup();
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

	DrawDockspaceMenuBar();
	DrawToolbar();

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
		ImGui::DockBuilderDockWindow("Game", dockMain);
		ImGui::DockBuilderDockWindow("Project", dockDown);

		ImGui::DockBuilderFinish(dockspaceID);
	}
	ImGui::End();
}
void EditorGUI::RenderGizmo(Camera* camera)
{
	if (!selectedObject.selectedObject)
		return;
	Transform* transform = selectedObject.selectedObject->GetComponent<Transform>();
	glm::mat4 view = camera->viewMatrix;
	glm::mat4 proj = camera->projection;

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (transform->parent)
	{
		modelMatrix = glm::inverse(transform->parent->GetModelMatrix()) * transform->GetModelMatrix();
	}
	else
		modelMatrix = transform->GetModelMatrix();
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
void EditorGUI::DrawHierarchyObject(GameObject* object)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if (object->transform->children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;
	bool selected = (selectedObject.selectedObject == object);
	if (selected)
		flags |= ImGuiTreeNodeFlags_Selected;
	ImGui::PushID(object);
	bool opened = ImGui::TreeNodeEx(object->name.c_str(), flags);
	if(ImGui::IsItemClicked())
	{
		selectedTexture = nullptr;
		selectedCubemap = nullptr;
		selectedMaterial = nullptr;
		selectedAsset = SelectedAsset::None;
		selectedObject.selectedObject = object;
		selectedObject.selectedObjectRender = object->GetComponent<Render>();
	}

	if (opened)
	{
		for (Transform* child : object->transform->children)
		{
			if (child->gameObject)
			{
				DrawHierarchyObject(child->gameObject);
			}
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}
void EditorGUI::ShowHierarchyWindow()
{

	ImGui::Begin("Hierarchy");
	for (std::unique_ptr<GameObject>& object : SceneManager::Get().GetActiveScene().objects)
	{
		if (object->name == "!##@@AENGINECAMERAEDITOR@@##!") continue;
		if (!object->transform->parent)
		{
			DrawHierarchyObject(object.get());
		}
	}
	ImGui::End();
}
void EditorGUI::ShowInspectorWindow()
{
	static GameObject* prevObj = nullptr;
	ImGui::Begin("Inspector");
	switch (selectedAsset)
	{
		case SelectedAsset::Material:
		{
			if (selectedMaterial)
			{
				ImGui::TextUnformatted(selectedMaterial->path.c_str());
				ImGui::Separator();
				bool writed = false;
				RenderMaterialPart(selectedMaterial, writed);
				if (writed)
					AssetManager::SaveMaterial(selectedMaterial, selectedMaterial->path.c_str());
			}
			break;
		}
		case SelectedAsset::Cubemap:
		{
			if (selectedCubemap)
			{
				ImGui::TextUnformatted(selectedCubemap->path.c_str());
				ImGui::Separator();

				float avaliableWidth = ImGui::GetContentRegionAvail().x;
				float previewWidth = std::min(avaliableWidth, 256.0f);

				auto SelectTextureForCubemap = [&](const char* ID, TypeTexture type)
					{
						ImGui::PushID(ID);
						ImGui::Button((std::string("Drop ") + std::string(ID)).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 60.0f));
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET"))
							{
								const char* relativePath = static_cast<const char*>(payload->Data);

								std::filesystem::path fullpath = projectDirectory / relativePath;

								std::string extension = fullpath.extension().string();

								std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char symbol) { return static_cast<char>(std::tolower(symbol)); });

								if (extension == ".jpg" || extension == ".png")
								{
									Texture* newTexture = AssetManager::LoadTexture(fullpath.generic_string());
									if (newTexture && newTexture->GetWidth() == selectedCubemap->size && newTexture->GetHeight() == selectedCubemap->size)
									{
										selectedCubemap->ChangeTexture(type, newTexture);
										AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
									}
								}
							}
							ImGui::EndDragDropTarget();
						}
						ImGui::PopID();
					};
				auto RightTexture = selectedCubemap->textures.find(TypeTexture::right);
				if (RightTexture != selectedCubemap->textures.end() && RightTexture->second)
				{
					ImGui::Image((ImTextureID)(uintptr_t)RightTexture->second->GetTexturePixels(), ImVec2(previewWidth, previewWidth), ImVec2(0, 0), ImVec2(1, 1));
				}
				SelectTextureForCubemap("Right", TypeTexture::right);

				ImGui::Separator();

				auto LeftTexture = selectedCubemap->textures.find(TypeTexture::left);
				if (LeftTexture != selectedCubemap->textures.end() && LeftTexture->second)
				{
					ImGui::Image((ImTextureID)(uintptr_t)LeftTexture->second->GetTexturePixels(), ImVec2(previewWidth, previewWidth), ImVec2(0, 0), ImVec2(1, 1));
				}
				SelectTextureForCubemap("Left", TypeTexture::left);
				ImGui::Separator();

				auto TopTexture = selectedCubemap->textures.find(TypeTexture::top);
				if (TopTexture != selectedCubemap->textures.end() && TopTexture->second)
				{
					ImGui::Image((ImTextureID)(uintptr_t)TopTexture->second->GetTexturePixels(), ImVec2(previewWidth, previewWidth), ImVec2(0, 0), ImVec2(1, 1));
				}
				SelectTextureForCubemap("Top", TypeTexture::top);
				ImGui::Separator();

				auto BottomTexture = selectedCubemap->textures.find(TypeTexture::bottom);
				if (BottomTexture != selectedCubemap->textures.end() && BottomTexture->second)
				{
					ImGui::Image((ImTextureID)(uintptr_t)BottomTexture->second->GetTexturePixels(), ImVec2(previewWidth, previewWidth), ImVec2(0, 0), ImVec2(1, 1));
				}
				SelectTextureForCubemap("Bottom", TypeTexture::bottom);
				ImGui::Separator();

				auto FrontTexture = selectedCubemap->textures.find(TypeTexture::front);
				if (FrontTexture != selectedCubemap->textures.end() && FrontTexture->second)
				{
					ImGui::Image((ImTextureID)(uintptr_t)FrontTexture->second->GetTexturePixels(), ImVec2(previewWidth, previewWidth), ImVec2(0, 0), ImVec2(1, 1));
				}
				SelectTextureForCubemap("Front", TypeTexture::front);
				ImGui::Separator();

				auto BackTexture = selectedCubemap->textures.find(TypeTexture::back);
				if (BackTexture != selectedCubemap->textures.end() && BackTexture->second)
				{
					ImGui::Image((ImTextureID)(uintptr_t)BackTexture->second->GetTexturePixels(), ImVec2(previewWidth, previewWidth), ImVec2(0, 0), ImVec2(1, 1));
				}
				SelectTextureForCubemap("Back", TypeTexture::back);

				if (ImGui::BeginCombo("Settings", std::to_string(selectedCubemap->size).c_str()))
				{
					if (ImGui::Selectable("16x16"))
					{
						selectedCubemap->ChangeSize(16);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					if (ImGui::Selectable("32x32"))
					{
						selectedCubemap->ChangeSize(32);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					if (ImGui::Selectable("64x64"))
					{
						selectedCubemap->ChangeSize(64);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					if (ImGui::Selectable("128x128"))
					{
						selectedCubemap->ChangeSize(128);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					if (ImGui::Selectable("256x256"))
					{
						selectedCubemap->ChangeSize(256);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					if (ImGui::Selectable("512x512"))
					{
						selectedCubemap->ChangeSize(512);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					if (ImGui::Selectable("1024x1024"))
					{
						selectedCubemap->ChangeSize(1024);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					if (ImGui::Selectable("2048x2048"))
					{
						selectedCubemap->ChangeSize(2048);
						AssetManager::SaveCubemap(selectedCubemap, selectedCubemap->path);
					}
					ImGui::EndCombo();
				}
			}
			break;
		}
		case SelectedAsset::Texture:
		{
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
			}
			break;
		}
		case SelectedAsset::None:
		{
			if (selectedObject.selectedObject)
			{
				AEngineComponent* removedComponent = nullptr;
				AE_COMPONENT_HANDLE removedComponentHandle = {};
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
								if (field.isColor)
									ImGui::ColorEdit3(field.name, glm::value_ptr(*value3));
								else
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
							case FieldType::VEC4:
							{
								auto* value4 = static_cast<glm::vec4*>(field.ptr);
								if (field.isColor)
									ImGui::ColorEdit4(field.name, glm::value_ptr(*value4));
								else
									ImGui::DragFloat4(field.name, glm::value_ptr(*value4), 0.1f);
								break;
							}
							}

						}
						ImGui::TreePop();
					}
				}
				int size = GameModule::GETGAMECONTEXT().getAComponentsCountFromGameObject(selectedObject.selectedObject->getHandle());
				std::vector<AE_COMPONENT_INFO> componentsinfo(size);
				GameModule::GETGAMECONTEXT().getAComponentsFromGameObject(selectedObject.selectedObject->getHandle(), componentsinfo.data(), size);

				for (unsigned int i = 0; i < size; i++)
				{
					ImGuiTreeNodeFlags flags =
						ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_Framed |
						ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_FramePadding;
					ImGui::PushID(i);
					bool opened = ImGui::TreeNodeEx("##GameComponent", flags, "%s", componentsinfo[i].name);
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("RemoveComponent"))
						{
							removedComponentHandle = componentsinfo[i].ae_component_handle;
						}
						ImGui::MenuItem("Reset");
						ImGui::MenuItem("Duplicate");

						ImGui::EndPopup();
					}
					if (opened)
					{
					
						for (int x = 0; x < componentsinfo[i].countFieldsInfo; x++)
						{
							AE_GAME_FIELD_INFO::FieldInfo& field = componentsinfo[i].fieldsInfo[x];
							switch (componentsinfo[i].fieldsInfo[x].typeValue)
							{
								case AE_GAME_FIELD_INFO::FieldType::BOOL:
								{
									ImGui::Checkbox(field.nameField, (bool*)field.ptr);
									break;
								}
								case AE_GAME_FIELD_INFO::FieldType::FLOAT:
								{
									if (field.maxValue > field.minValue)
										ImGui::SliderFloat(field.nameField, (float*)field.ptr, field.minValue, field.maxValue);
									else
										ImGui::DragFloat(field.nameField, (float*)field.ptr, 0.1f);
									break;
								}
								case AE_GAME_FIELD_INFO::FieldType::INT:
								{
									if ((int)field.maxValue > (int)field.minValue)
										ImGui::SliderInt(field.nameField, (int*)field.ptr, (int)field.minValue, (int)field.maxValue);
									else
										ImGui::DragInt(field.nameField, (int*)field.ptr, 1.0f);
									break;
								}
								case AE_GAME_FIELD_INFO::FieldType::VEC3:
								{
									auto* value3 = static_cast<AE_VEC3*>(field.ptr);
									if (field.isColor)
										ImGui::ColorEdit3(field.nameField, &value3->x);
									else
										ImGui::DragFloat3(field.nameField, &value3->x, 0.1f);
									break;
								}
								case AE_GAME_FIELD_INFO::FieldType::VEC4:
								{
									auto* value4 = static_cast<AE_VEC4*>(field.ptr);
									if (field.isColor)
										ImGui::ColorEdit4(field.nameField, &value4->x);
									else
										ImGui::DragFloat4(field.nameField, &value4->x, 0.1f);
									break;
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				if (auto* selectedTransform = selectedObject.selectedObject->GetComponent<Transform>())
				{
					selectedTransform->UpdateMatrices();
				}
				GameModule::GETGAMECONTEXT().removeAComponent(selectedObject.selectedObject->getHandle(), removedComponentHandle);
				if (removedComponent)
				{
					selectedObject.selectedObject->RemoveComponent(removedComponent);
					removedComponent = nullptr;
				}
				Render* renderComponent = selectedObject.selectedObject->GetComponent<Render>();
				if (renderComponent && renderComponent->material)
				{
					bool writed = false;
					RenderMaterialPart(renderComponent->material, writed);
					if (writed)
						AssetManager::SaveMaterial(renderComponent->material, renderComponent->material->path);
					ImGui::Text("Select material");
					std::string materialName = "None";
					if (renderComponent && renderComponent->material)
						materialName = renderComponent->material->path;
					ImGui::Button(materialName.c_str(), ImVec2(200, 30));

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET"))
						{
							const char* relativePath = static_cast<const char*>(payload->Data);
							std::filesystem::path fullpath = projectDirectory / relativePath;
							std::string extension = fullpath.extension().string();
							std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {return static_cast<char>(std::tolower(c)); });
							if (extension == ".amat" && MaterialSerializer::IsMaterial(fullpath.generic_string()))
							{
								renderComponent->material = AssetManager::LoadMaterial(fullpath.generic_string());
							}
						}
						ImGui::EndDragDropTarget();
					}
				}
				if (ImGui::BeginCombo("AddComponent", "Select"))
				{
					for (auto& component : ComponentSystem::GetComponentsFabric())
					{
						if (component.first == "Transform") continue;
						if (ImGui::Selectable(component.first.c_str()))
						{
							component.second(selectedObject.selectedObject);
						}
					}
					if (GameModule::GETGAMECONTEXT().getRegisteredAComponentsCount)
					{
						for (unsigned int i = 0; i < GameModule::GETGAMECONTEXT().getRegisteredAComponentsCount(); i++)
						{
							if (ImGui::Selectable(GameModule::GETGAMECONTEXT().getRegisteredAComponentName(i)))
							{
								GameModule::GETGAMECONTEXT().addAComponent(i, selectedObject.selectedObject->getHandle());
							}
						}
					}
					ImGui::EndCombo();
				}
			}
			else
			{
				transform = nullptr;
				prevObj = nullptr;
			}
			break;
		}
	}
	ImGui::End();
}
void EditorGUI::RenderMaterialPart(Material* material, bool &writed)
{
	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_FramePadding;
	if (material)
	{
		bool opened = ImGui::TreeNodeEx(material, flags, "%s", material->path.c_str());
		if (opened)
		{
			ImGui::BeginDisabled(!material->hasFile);
			if (ImGui::BeginCombo("Shader", (material->GetShader()->GetPathVertexShader() + "|" + material->GetShader()->GetPathFragmentShader() + (material->GetShader()->GetPathGeometryShader() != "" ? "|" + material->GetShader()->GetPathGeometryShader() : "")).c_str()))
			{
				for (const auto& shader : AssetManager::GetShaders())
				{
					bool selected = material->GetShader() == shader.second.get();
					if (ImGui::Selectable(shader.first.c_str(), selected))
					{
						if (material->hasFile)
						{
							material->SetShader(shader.second.get());
							writed = true;
						}
					}
				}
				ImGui::EndCombo();
			}
			for (auto& fieldmaterial : material->GetProperties()->propertiesNames)
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
								if (material->hasFile)
								{
									material->GetProperties()->SetValue<float>(fieldmaterial, temp);
									writed = true;
								}
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
								if (material->hasFile)
								{
									material->GetProperties()->SetValue<float>(fieldmaterial, temp);
									writed = true;
								}
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
								if (material->hasFile)
								{
									material->GetProperties()->SetValue<int>(fieldmaterial, temp);
									writed = true;
								}
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
								if (material->hasFile)
								{
									material->GetProperties()->SetValue<int>(fieldmaterial, temp);
									writed = true;
								}
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
							if (material->hasFile)
							{
								material->GetProperties()->SetValue<bool>(fieldmaterial, temp);
								writed = true;
							}
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
									if (material->hasFile)
									{
										material->GetProperties()->SetValue<GLSLParserMath::Vector4>(fieldmaterial, GLSLParserMath::Vector4::float4_to_vector4(color));
										writed = true;
									}
								}
							}
							else
							{
								if (ImGui::ColorEdit4(fieldmaterial.c_str(), color))
								{
									if (material->hasFile)
									{
										material->GetProperties()->SetValue<GLSLParserMath::Vector4>(fieldmaterial, GLSLParserMath::Vector4::float4_to_vector4(color));
										writed = true;
									}
								}
							}
						}
						else
						{
							if (ImGui::DragFloat4(fieldmaterial.c_str(), color, 0.1f))
							{
								if (material->hasFile)
								{
									material->GetProperties()->SetValue<GLSLParserMath::Vector4>(fieldmaterial, GLSLParserMath::Vector4::float4_to_vector4(color));
									writed = true;
								}
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
							if (material->hasFile)
							{
								material->GetProperties()->SetValue<GLSLParserMath::Vector3>(fieldmaterial, GLSLParserMath::Vector3::float3_to_vector3(color));
								writed = true;
							}
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
							if (material->hasFile)
							{
								material->GetProperties()->SetValue<GLSLParserMath::Vector2>(fieldmaterial, GLSLParserMath::Vector2::float2_to_vector2(color));
								writed = true;
							}
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
							if (material->hasFile)
							{
								material->Set(fieldmaterial, (Texture*)nullptr);
								writed = true;
							}
						}
						for (auto& texture : AssetManager::GetTextures())
						{
							if (ImGui::Selectable(texture.first.c_str()))
							{
								if (material->hasFile)
								{
									material->Set(fieldmaterial, texture.second.get());
									writed = true;
								}
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
									if (material->hasFile)
									{
										material->Set(fieldmaterial, newTexture);
										writed = true;
									}
							}
						}
						ImGui::EndDragDropTarget();
					}
				}
				else if (fieldmaterialinfo->second.info.typefield == PGLinfos::FieldType::SamplerCube)
				{
					std::string comboId = fieldmaterial;
					Cubemap* selectedCubemap = material->GetCubemap(fieldmaterial);
					const char* preview = selectedCubemap ? selectedCubemap->path.c_str() : "None";
					ImGui::BeginGroup();
					if (ImGui::BeginCombo(comboId.c_str(), preview))
					{
						if (ImGui::Selectable("None"))
						{
							if (material->hasFile)
							{
								material->Set(fieldmaterial, (Cubemap*)nullptr);
								writed = true;
							}
						}
						for (auto& texture : AssetManager::GetCubemaps())
						{
							if (ImGui::Selectable(texture.first.c_str()))
							{
								if (material->hasFile)
								{
									material->Set(fieldmaterial, texture.second.get());
									writed = true;
								}
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

							if (extension == ".acubemap" && CubemapSerializer::IsCubemap(fullpath.generic_string()))
							{
								Cubemap* cubemap = AssetManager::LoadCubemap(fullpath.generic_string());
								if (cubemap)
									if (material->hasFile)
									{
										material->Set(fieldmaterial, cubemap);
										writed = true;
									}
							}

						}
						ImGui::EndDragDropTarget();
					}
				}
			}
			ImGui::EndDisabled();
			ImGui::TreePop();
		}
	}
}
