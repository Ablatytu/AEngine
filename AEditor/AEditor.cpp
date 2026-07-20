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
	Shader* explodingStandartShader = AssetManager::LoadShader("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl", "Shaders/ExplodingGeometryShader.glsl");
	Shader* toonShader = AssetManager::LoadShader("Assets/ToonshaderVS.glsl", "Assets/ToonshaderFS.glsl");

	Cubemap* cubeMapDefaultSkyBox = AssetManager::LoadCubemap("cubeMap", 2048);
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

	Material* material = AssetManager::CreateMaterial(standartShader);
	Material* terrainMaterial = AssetManager::CreateMaterial(standartShader);
	Material* cubeMaterial = AssetManager::CreateMaterial(cubeTransparent);
	Material* CubemapMaterial = AssetManager::CreateMaterial(cubemapShader);
	Material* ExplodingMaterial = AssetManager::CreateMaterial(explodingStandartShader);
	Material* planeMaterial = AssetManager::CreateMaterial(standartShader);
	Material* ToonMaterial = AssetManager::CreateMaterial(toonShader);
	Material* spherePBRMaterial = AssetManager::CreateMaterial(standartShader);

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

	ExplodingMaterial->Set("texture_diffuse1", texture);

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
	SceneManager::Get().CreateScene();
	SceneManager::Get().LoadScene(0);
	SceneManager::Get().GetActiveScene().SetSkyBox(enviromentTexture);
	GameObject* Plush_rei = SceneManager::Get().CreateGameObject();
	GameObject* Plush_rei1 = SceneManager::Get().CreateGameObject();
	GameObject* Plush_rei2 = SceneManager::Get().CreateGameObject();
	GameObject* camera = SceneManager::Get().CreateGameObject();
	GameObject* directionalLight = SceneManager::Get().CreateGameObject();
	GameObject* pointLight = SceneManager::Get().CreateGameObject();
	GameObject* pointLightR = SceneManager::Get().CreateGameObject();
	GameObject* pointLightG = SceneManager::Get().CreateGameObject();
	GameObject* spotLight = SceneManager::Get().CreateGameObject();
	GameObject* terrain = SceneManager::Get().CreateGameObject();
	GameObject* Cube3D = SceneManager::Get().CreateGameObject();
	GameObject* plane = SceneManager::Get().CreateGameObject();
	std::cout << "COMPLITE\n";
	unsigned int amount = 1000;
	unsigned int maxLights = 192;
	unsigned int step = amount / maxLights;
	unsigned int createdLights = 0;
	srand(glfwGetTime());
	float radius = 15;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; i++)
	{
		std::cout << "CREATE REI INSANCE: " << i << "\n";
		GameObject* Plush_reiInstancing = SceneManager::Get().CreateGameObject();
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		Plush_reiInstancing->GetComponent<Transform>()->position = glm::vec3(x, y, z);
		float scale = (rand() % 20) / 100.0f + 0.05;
		Plush_reiInstancing->GetComponent<Transform>()->scale = glm::vec3(scale);
		float rotAngle = (rand() % 360);
		Plush_reiInstancing->GetComponent<Transform>()->rotation = Quaternion::Euler(rotAngle, rotAngle, rotAngle);
		if (createdLights < maxLights && i % step == 0)
		{
			float t = (float)createdLights / (float)maxLights;
			Plush_reiInstancing->AddComponent<Light>();
			Plush_reiInstancing->GetComponent<Light>()->typeLight = Light::Type::Point;
			Plush_reiInstancing->GetComponent<Light>()->lightColor = glm::vec3(0.5f + 0.5f * sin(t * 6.28318f), 0.5f + 0.5f * sin(t * 6.28318f + 2.094f), 0.5f + 0.5f * sin(t * 6.28318f + 4.188f));
			Plush_reiInstancing->GetComponent<Light>()->intensive = 3.0f;
			createdLights++;
		}
		Plush_reiInstancing->name = std::string("PlushReiInstance ") + std::to_string(i);
		Plush_reiInstancing->AddComponent<Render>();
		Plush_reiInstancing->AddComponent<Cube>();
		Plush_reiInstancing->GetComponent<Cube>()->rotate = true;
		Plush_reiInstancing->GetComponent<Render>()->mesh = &rei->meshes[0];
		Plush_reiInstancing->GetComponent<Render>()->material = material;
		Plush_reiInstancing->GetComponent<Transform>()->UpdateMatrices();
	}

	GameObject* newSphere = SceneManager::Get().CreateGameObject();
	newSphere->AddComponent<Render>();
	newSphere->GetComponent<Render>()->mesh = &sphere->meshes[0];
	newSphere->GetComponent<Render>()->material = spherePBRMaterial;
	newSphere->GetComponent<Transform>()->position = glm::vec3(2.0f, 2.0f, 5.0f);
	newSphere->GetComponent<Transform>()->scale = glm::vec3(0.5f);
	newSphere->GetComponent<Transform>()->UpdateMatrices();

	Plush_rei->name = "PlushRei";
	Plush_rei1->name = "PlushRei1";
	Plush_rei2->name = "PlushRei2";
	camera->name = "Camera";
	directionalLight->name = "directionalLight";
	pointLight->name = "pointLightB";
	pointLightG->name = "pointLightG";
	spotLight->name = "spotLight";
	terrain->name = "Terrain";
	Cube3D->name = "Cube";
	pointLightR->name = "pointLightR";
	plane->AddComponent<Render>();
	plane->GetComponent<Render>()->mesh = &planeModel->meshes[0];
	plane->GetComponent<Render>()->material = planeMaterial;
	plane->GetComponent<Transform>()->position = glm::vec3(0.0f, 0.0f, 0.0f);
	plane->GetComponent<Transform>()->scale = glm::vec3(5.0f, 1.0f, 5.0f);
	plane->GetComponent<Transform>()->UpdateMatrices();
	Cube3D->AddComponent<Render>();
	Cube3D->GetComponent<Render>()->mesh = &cube->meshes[0];
	Cube3D->GetComponent<Render>()->material = CubemapMaterial;
	Cube3D->GetComponent<Transform>()->position = glm::vec3(5.0f, 1.0f, -3.0f);
	Cube3D->GetComponent<Transform>()->UpdateMatrices();
	terrain->AddComponent<Render>();
	terrain->GetComponent<Render>()->mesh = &terrainModel->meshes[0];
	terrain->GetComponent<Render>()->material = terrainMaterial;
	terrain->GetComponent<Transform>()->scale = glm::vec3(0.005f, 0.005f, 0.005f);
	terrain->GetComponent<Transform>()->position = glm::vec3(5.0f, 0.0f, 3.0f);
	terrain->GetComponent<Transform>()->UpdateMatrices();
	spotLight->AddComponent<Light>();
	spotLight->GetComponent<Light>()->typeLight = Light::Type::Spot;
	spotLight->GetComponent<Transform>()->position = glm::vec3(0.0f, 2.0f, 3.0f);
	spotLight->GetComponent<Light>()->intensive = 1.0f;
	spotLight->GetComponent<Light>()->lightColor = glm::vec3(1.0f, 0.5f, 1.0f);
	spotLight->AddComponent<Cube>();
	spotLight->AddComponent<Render>();
	spotLight->GetComponent<Render>()->mesh = &cube->meshes[0];
	spotLight->GetComponent<Render>()->material = cubeMaterial;
	spotLight->GetComponent<Transform>()->UpdateMatrices();
	pointLight->AddComponent<Light>();
	pointLight->GetComponent<Light>()->typeLight = Light::Type::Point;
	pointLight->GetComponent<Light>()->lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight->GetComponent<Transform>()->position = glm::vec3(4.0f, 1.0f, 1.5f);
	pointLight->GetComponent<Light>()->intensive = 1.0f;
	pointLight->GetComponent<Light>()->shadow = true;
	pointLight->AddComponent<LightMoving>();
	pointLight->AddComponent<Render>();
	//pointLight->GetComponent<Render>()->mesh = &cube->meshes[0];
	//pointLight->GetComponent<Render>()->material = &cubeMaterial;
	pointLightR->AddComponent<Render>();
	pointLightR->GetComponent<Render>()->mesh = &cube->meshes[0];
	pointLightR->GetComponent<Render>()->material = cubeMaterial;
	pointLightR->GetComponent<Transform>()->position = glm::vec3(0.0f, 3.0f, 1.5f);
	pointLightR->AddComponent<Light>();
	pointLightR->GetComponent<Light>()->typeLight = Light::Type::Point;
	pointLightR->GetComponent<Light>()->lightColor = glm::vec3(1.0f, 0.0f, 1.0f);
	pointLightR->GetComponent<Light>()->intensive = 1.0f;
	pointLightR->AddComponent<LightMoving>();
	pointLightR->GetComponent<LightMoving>()->rigntPos = true;
	pointLightG->AddComponent<Render>();
	pointLightG->GetComponent<Render>()->mesh = &cube->meshes[0];
	pointLightG->GetComponent<Render>()->material = cubeMaterial;
	pointLightG->GetComponent<Transform>()->position = glm::vec3(0.0f, 2.0f, -45.5f);
	pointLightG->AddComponent<Light>();
	pointLightG->GetComponent<Light>()->typeLight = Light::Type::Point;
	pointLightG->GetComponent<Light>()->lightColor = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLightG->GetComponent<Light>()->intensive = 0.0f;
	pointLightG->AddComponent<LightMoving>();
	pointLightG->GetComponent<LightMoving>()->CanUp = true;
	pointLight->GetComponent<Transform>()->UpdateMatrices();
	pointLightR->GetComponent<Transform>()->UpdateMatrices();
	pointLightG->GetComponent<Transform>()->UpdateMatrices();
	directionalLight->AddComponent<Light>();
	directionalLight->GetComponent<Light>()->typeLight = Light::Type::Directional;
	directionalLight->GetComponent<Light>()->intensive = 1.0f;
	directionalLight->GetComponent<Light>()->shadow = true;
	directionalLight->GetComponent<Transform>()->position = glm::vec3(-1.0f, 6.0f, 0.5f);
	directionalLight->GetComponent<Transform>()->rotation = Quaternion::Euler(-45.0f, 45.0f, 45.0f);
	Plush_rei->AddComponent<Render>();
	Plush_rei->AddComponent<Rei>();
	Plush_rei->GetComponent<Render>()->mesh = &rei->meshes[0];
	Plush_rei->GetComponent<Render>()->material = ToonMaterial;
	Plush_rei->GetComponent<Transform>()->position = glm::vec3(-2.0f, -0.2f, 0.5f);
	Plush_rei->GetComponent<Transform>()->UpdateMatrices();
	Plush_rei->AddComponent<Cube>();
	Plush_rei1->AddComponent<Render>();
	Plush_rei1->GetComponent<Render>()->mesh = &rei->meshes[0];
	Plush_rei1->GetComponent<Render>()->material = material;
	Plush_rei1->GetComponent<Transform>()->position = glm::vec3(2.0f, -0.2f, 0.5f);
	Plush_rei1->GetComponent<Transform>()->UpdateMatrices();
	//Plush_rei1.AddComponent<Cube>();
	Plush_rei2->AddComponent<Render>();
	Plush_rei2->GetComponent<Render>()->mesh = &rei->meshes[0];
	Plush_rei2->GetComponent<Render>()->material = material;
	Plush_rei2->GetComponent<Transform>()->position = glm::vec3(0.0f, -0.2f, 0.5f);
	Plush_rei2->GetComponent<Transform>()->UpdateMatrices();
	//Plush_rei2.AddComponent<Cube>();
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
	SceneManager::Get().GetActiveScene().HDR = true;
	auto renderContext = RenderContext{};
	std::cout << "END INI";
}

void AEditor::OnBeforeSceneUpdate()
{
	//frame++;
	//if (frame > 10)
	//	close();
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