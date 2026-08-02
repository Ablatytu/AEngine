#include <iostream>
#include <core/Application.h>
#include "AEditor.h"

#include "../AEngine/Renderer/Mesh.h"
#include "../AEngine/Renderer/Shader.h"
#include "../AEngine/components/Render.h"
#include "../AEngine/Renderer/GameObject.h"
#include "../AEngine/Renderer/Texture.h"
#include "../AEngine/Renderer/Material.h"
#include "../AEngine/components/cube.h"
#include "EditorCamera.h"
#include "../AEngine/APIs/ModelLoader.h"
#include "../AEngine/components/LightMoving.h"
#include <core/Window.h>
#include "../AEngine/components/Rei.h"
#include "../AEngine/scene/AssetManager.h"
#include <map>
#include <cmath>
#include "../AEngine/Renderer/Framebuffer.h"
#include "../AEngine/Renderer/FramebufferManager.h"
#include "../AEngine/scene/Skybox.h"
#include "../AEngine/Renderer/Cubemap.h"
#include "../AEngine/Renderer/Buffer.h"
#include "../AEngine/Renderer/RenderSystem.h"
#include "../AEngine/Renderer/RenderContext.h"
#include "../AEngine/components/PostProcess.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Editor.h"

using namespace RenderSystem;

void AEditor::OnInit()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplGlfw_InitForOpenGL(Application::Get().GetWindow().window, true);
	ImGui_ImplOpenGL3_Init();
	editorGUI = std::make_unique<EditorGUI>();
	Model* rei = AssetManager::LoadModel("Assets/Models/ReiPlush.obj");
	Model* terrainModel = AssetManager::LoadModel("Assets/Models/8842eed260aa4dad81773d51ca9066fb.obj");
	Model* cube = AssetManager::LoadModel("Assets/Models/Untitled.obj");
	Model* planeModel = AssetManager::LoadModel("Assets/Models/Plane.obj");
	Model* sphere = AssetManager::LoadModel("Assets/Models/Sphere.obj");

	Shader* standartShader = AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShaderDeferredGeometryTest.glsl");
	Shader* cubeTransparent = AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");
	Shader* cubemapShader = AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/CubemapShader.glsl");
	//Shader* explodingStandartShader = AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl", "Shaders/ExplodingGeometryShader.glsl");
	Shader* toonShader = AssetManager::LoadShader("Assets/ToonshaderVS.glsl", "Assets/ToonshaderFS.glsl");

	Cubemap* cubeMapDefaultSkyBox = AssetManager::CreateCubemap("cubeMap", 2048);
	Texture* textureRight = AssetManager::LoadTexture("Assets/Textures/skybox/right.jpg");
	Texture* textureLeft = AssetManager::LoadTexture("Assets/Textures/skybox/left.jpg");
	Texture* textureTop = AssetManager::LoadTexture("Assets/Textures/skybox/top.jpg");
	Texture* texturebottom = AssetManager::LoadTexture("Assets/Textures/skybox/bottom.jpg");
	Texture* textureFront = AssetManager::LoadTexture("Assets/Textures/skybox/front.jpg");
	Texture* textureBack = AssetManager::LoadTexture("Assets/Textures/skybox/back.jpg");
	HDRTexture* enviromentTexture = AssetManager::LoadHDRTexture("Assets/Textures/HDR/pergola_walkway_2k.hdr");
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::right, textureRight);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::left, textureLeft);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::top, textureTop);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::bottom, texturebottom);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::front, textureFront);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::back, textureBack);

	Material* material = AssetManager::CreateMaterial("StandartMaterial", standartShader);
	Material* terrainMaterial = AssetManager::CreateMaterial("TerrainMaterial", standartShader);
	Material* cubeMaterial = AssetManager::CreateMaterial("CubeMaterial", cubeTransparent);
	Material* CubemapMaterial = AssetManager::CreateMaterial("CubemapMaterial", cubemapShader);
	//Material* ExplodingMaterial = AssetManager::CreateMaterial("ExplodingMaterial", explodingStandartShader);
	Material* planeMaterial = AssetManager::CreateMaterial("PlaneMaterial", standartShader);
	Material* ToonMaterial = AssetManager::CreateMaterial("ToonMaterial", toonShader);
	Material* spherePBRMaterial = AssetManager::CreateMaterial("spherePBRMaterial", standartShader);

	Texture* texture = AssetManager::LoadTexture("Assets/Textures/d8c637d6-f500-44a3-bf33-c7896b95f8a0.png");
	Texture* terrainTexture = AssetManager::LoadTexture("Assets/Textures/diff2.jpg");
	Texture* cubeTexture = AssetManager::LoadTexture("Assets/Textures/container.jpg");
	Texture* PlaneTexture = AssetManager::LoadTexture("Assets/Textures/brickwall.jpg");
	Texture* PlaneTextureNormal = AssetManager::LoadTexture("Assets/Textures/brickwall_normal.jpg", true);
	Texture* CobbleStoneTexture = AssetManager::LoadTexture("Assets/Textures/cobblestone_floor_09_diff_1k.jpg");
	Texture* CobbleStoneNormal = AssetManager::LoadTexture("Assets/Textures/cobblestone_floor_09_nor_dx_1k.jpg", true);
	Texture* BrickWall = AssetManager::LoadTexture("Assets/Textures/bricks2.jpg");
	Texture* BrickWallHeight = AssetManager::LoadTexture("Assets/Textures/bricks2_disp.jpg", true);
	Texture* BrickWallNormal = AssetManager::LoadTexture("Assets/Textures/bricks2_normal.jpg", true);

	ToonMaterial->Set("_MainTexture", texture);

	//ExplodingMaterial->Set("texture_diffuse1", texture);

	planeMaterial->Set("texture_diffuse1", BrickWall);
	planeMaterial->Set("normalMap", PlaneTextureNormal);
	planeMaterial->Set("coefficientNormal", 0.0f);
	planeMaterial->Set("heightMap", BrickWallHeight);
	planeMaterial->Set("height_scale", 0.1f);

	material->Set("GPUInstancing", true);
	material->Set("texture_diffuse1", texture);
	terrainMaterial->Set("texture_diffuse1", terrainTexture);
	cubeMaterial->surfaceType = Material::SurfaceType::Transparent;
	cubeMaterial->Set("texture_diffuse1", cubeTexture);
	cubeMaterial->Set("objectColor", glm::vec4(0.8f, 0.5f, 1.0f, 0.5f));
	CubemapMaterial->Set("cubemap", cubeMapDefaultSkyBox);

	spherePBRMaterial->Set("metallic", 1.0f);
	spherePBRMaterial->Set("roughness", 0.0f);

	rs_Init();
	editorGUI->EDITOR_INIT();
	std::cout << "LOAD SCENE AND GAMEOBJECTS...";
	SceneManager::Get().LoadScene("Assets/TestScene.ascene");
}
void AEditor::OnSceneLoaded()
{
	GameObject* camera = SceneManager::Get().CreateGameObject();
	camera->name = "!##@@AENGINECAMERAEDITOR@@##!";
	camera->AddComponent<Camera>();
	camera->GetComponent<Transform>()->position = glm::vec3(2.0f, 2.0f, 7.0f);
	camera->GetComponent<Transform>()->rotation = Quaternion::Euler(0.0f, 0.0f, 0.0f);
	camera->GetComponent<Transform>()->UpdateMatrices();
	camera->AddComponent<PostProcess>();
	camera->GetComponent<PostProcess>()->exposure = 1.0f;
	camera->GetComponent<PostProcess>()->AutoExposure = true;
	std::cout << "CREATE EDITOR CAMERA...";
	editorCamera = std::make_unique<EditorCamera>(camera, editorGUI.get());
	std::cout << "COMPLITE\n";
}
void AEditor::OnBeforeSceneUpdate()
{
	//frame++;
	//if (frame > 10)
	//	close();
	if (SceneManager::Get().CheckChangeScenePathState())
	{
		glm::vec3 pos = glm::vec3(2.0f, 2.0f, 7.0f);
		glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		float yaw = 0.0f, pitch = 0.0f;

		if (editorCamera)
		{
			pos = editorCamera->transform->position;
			rot = editorCamera->transform->rotation;
			yaw = editorCamera->yaw;
			pitch = editorCamera->pitch;
			editorCamera.reset();
		}
		SceneManager::Get().ChangeScene();
		OnSceneLoaded();
		editorCamera->transform->position = pos;
		editorCamera->transform->rotation = rot;
		editorCamera->yaw = yaw;
		editorCamera->pitch = pitch;
		editorCamera->transform->UpdateMatrices();
	}
	Input::SetViewport(editorGUI->viewportBounds[0][0], editorGUI->viewportBounds[0][1], editorGUI->sizeviewport[0], editorGUI->sizeviewport[1]);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (editorCamera)
		editorCamera->Update();
}
void AEditor::OnRender()
{
	FramebufferManager::Get().Resize(editorGUI->sizeviewport[0], editorGUI->sizeviewport[1]);
	//std::cout << editorGUI->sizeviewport[0] << " " << editorGUI->sizeviewport[1] << "\n";
	RenderContext renderContext = SceneManager::Get().GetActiveScene().CollectRenderContext();
	renderContext.width = editorGUI->sizeviewport[0];
	renderContext.height = editorGUI->sizeviewport[1];
	if (editorGUI && editorGUI->selectedObject.selectedObjectRender)
		renderContext.selectedObject = editorGUI->selectedObject.selectedObjectRender;
	else
		renderContext.selectedObject = nullptr;
	if (renderContext.camera)
		renderContext.camera->SetViewport(renderContext.width, renderContext.height);
	rs_Start(editorGUI->sceneHDR);
	rs_Render(renderContext, editorGUI->sceneHDR);
	editorGUI->RenderEditorParts();
	rs_End();
	rs_PostProcess(renderContext, editorGUI->sceneHDR);
	rs_Present(renderContext, editorGUI->sceneHDR, editorGUI->viewport);
	editorGUI->BeginDockspace();
	editorGUI->ShowInspectorWindow();
	editorGUI->ShowHierarchyWindow();
	editorGUI->ShowProject();
	editorGUI->RenderViewport();
	editorGUI->ShowLightingMenu();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void AEditor::OnShutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
AEditor::AEditor() = default;
AEditor::~AEditor() = default;