#include "Application.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Shader.h"
#include "../components/Render.h"
#include "../Renderer/GameObject.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Material.h"
#include "../components/cube.h"
#include "../components/EditorCamera.h"
#include "../APIs/ModelLoader.h"
#include "../components/LightMoving.h"
#include "../Editor/Editor.h"
#include "Window.h"
#include "../components/Rei.h"
#include "../scene/AssetManager.h"
#include <map>
#include <cmath>
#include <AMath.h>
#include "../Renderer/Framebuffer.h"
#include "../Renderer/FramebufferManager.h"
#include "../scene/Skybox.h"
#include "../Renderer/Cubemap.h"
#include "../Renderer/Buffer.h"
#include "../Renderer/RenderSystem.h"
#include "../Renderer/RenderContext.h"
#include "../components/PostProcess.h"
using namespace RenderSystem;
Application* Application::app = nullptr;
Application::Application()
{
	if (!app)
		app = this;
}
Application& Application::Get()
{
	return *app;
}
int Application::run()
{
	Model* rei = AssetManager::LoadModel("Assets/Models/ReiPlush.obj");
	Model* terrainModel = AssetManager::LoadModel("Assets/Models/8842eed260aa4dad81773d51ca9066fb.obj");
	Model* cube = AssetManager::LoadModel("Assets/Models/Untitled.obj");
	Model* planeModel = AssetManager::LoadModel("Assets/Models/Plane.obj");
	Model* cave = AssetManager::LoadModel("Assets/Models/LP.obj");
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
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::right, textureRight);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::left, textureLeft);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::top, textureTop);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::bottom, texturebottom);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::front, textureFront);
	cubeMapDefaultSkyBox->ChangeTexture(TypeTexture::back, textureBack);

	Material material;
	Material terrainMaterial;
	Material cubeMaterial;
	Material CubemapMaterial;
	Material ExplodingMaterial;
	Material planeMaterial;
	Material ToonMaterial;
	Material CaveMaterial;

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
	Texture* CaveTexture = AssetManager::LoadTexture("Assets/Textures/LP_cave1_BaseColor.png");
	Texture* CaveNormalTexture = AssetManager::LoadTexture("Assets/Textures/LP_cave1_Normal.png", true);

	ToonMaterial.SetShader(toonShader);
	ToonMaterial.Set("_MainTexture", texture);

	ExplodingMaterial.SetShader(explodingStandartShader);
	ExplodingMaterial.Set("texture_diffuse1", texture);
	ExplodingMaterial.Set("coefficientCubemap", 0.0f);
	ExplodingMaterial.Set("Cubemap", cubeMapDefaultSkyBox);

	planeMaterial.SetShader(standartShader);
	planeMaterial.Set("texture_diffuse1", BrickWall);
	planeMaterial.Set("normalMap", PlaneTextureNormal);
	planeMaterial.Set("coefficientNormal", 0.0f);
	planeMaterial.Set("heightMap", BrickWallHeight);
	planeMaterial.Set("height_scale", 0.1f);
	planeMaterial.Set("coefficientCubemap", 0.0f);

	material.SetShader(standartShader);
	material.Set("GPUInstancing", true);
	material.Set("texture_diffuse1", texture);
	material.Set("coefficientCubemap", 0.0f);
	material.Set("Cubemap", cubeMapDefaultSkyBox);
	terrainMaterial.SetShader(standartShader);
	terrainMaterial.Set("texture_diffuse1", terrainTexture);
	terrainMaterial.Set("coefficientCubemap", 0.0f);
	terrainMaterial.Set("Cubemap", cubeMapDefaultSkyBox);
	cubeMaterial.surfaceType = Material::SurfaceType::Transparent;
	cubeMaterial.SetShader(cubeTransparent);
	cubeMaterial.Set("texture_diffuse1", cubeTexture);
	cubeMaterial.Set("coefficientCubemap", 0.0f);
	cubeMaterial.Set("objectColor", glm::vec4(0.8f, 0.5f, 1.0f, 0.5f));
	CubemapMaterial.SetShader(cubemapShader);
	CubemapMaterial.Set("cubemap", cubeMapDefaultSkyBox);

	CaveMaterial.SetShader(standartShader);
	CaveMaterial.Set("texture_diffuse1", CaveTexture);
	CaveMaterial.Set("coefficientCubemap", 0.0f);
	CaveMaterial.Set("Cubemap", cubeMapDefaultSkyBox); 
	CaveMaterial.Set("normalMap", CaveNormalTexture);
	CaveMaterial.Set("coefficientNormal", 1.0f);

	SceneManager::Get().CreateScene();
	SceneManager::Get().LoadScene(0);
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
	GameObject* caveObject = SceneManager::Get().CreateGameObject();

	unsigned int amount = 1000;
	unsigned int maxLights = 192;
	unsigned int step = amount / maxLights;
	unsigned int createdLights = 0;
	srand(glfwGetTime());
	float radius = 15;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; i++)
	{
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
			Plush_reiInstancing->GetComponent<Light>()->lightColor = glm::vec3(0.5f + 0.5f * sin(t * 6.28318f),0.5f + 0.5f * sin(t * 6.28318f + 2.094f), 0.5f + 0.5f * sin(t * 6.28318f + 4.188f));
			Plush_reiInstancing->GetComponent<Light>()->intensive = 1.0f;
			createdLights++;
		}
		Plush_reiInstancing->name = std::string("PlushReiInstance ") + std::to_string(i);
		Plush_reiInstancing->AddComponent<Render>();
		Plush_reiInstancing->AddComponent<Cube>();
		Plush_reiInstancing->GetComponent<Cube>()->rotate = true;
		Plush_reiInstancing->GetComponent<Render>()->mesh = &rei->meshes[0];
		Plush_reiInstancing->GetComponent<Render>()->material = &material;
		Plush_reiInstancing->GetComponent<Transform>()->UpdateMatrices();
	}
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
	plane->GetComponent<Render>()->material = &planeMaterial;
	plane->GetComponent<Transform>()->position = glm::vec3(0.0f, -0.5f, 0.0f);
	plane->GetComponent<Transform>()->scale = glm::vec3(5.0f, 1.0f, 5.0f);
	plane->GetComponent<Transform>()->UpdateMatrices();
	Cube3D->AddComponent<Render>();
	Cube3D->GetComponent<Render>()->mesh = &cube->meshes[0];
	Cube3D->GetComponent<Render>()->material = &CubemapMaterial;
	Cube3D->GetComponent<Transform>()->position = glm::vec3(5.0f, 1.0f, -3.0f);
	Cube3D->GetComponent<Transform>()->UpdateMatrices();
	terrain->AddComponent<Render>();
	terrain->GetComponent<Render>()->mesh = &terrainModel->meshes[0];
	terrain->GetComponent<Render>()->material = &terrainMaterial;
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
	spotLight->GetComponent<Render>()->material = &cubeMaterial;
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
	pointLightR->GetComponent<Render>()->material = &cubeMaterial;
	pointLightR->GetComponent<Transform>()->position = glm::vec3(0.0f, 3.0f, 1.5f);
	pointLightR->AddComponent<Light>();
	pointLightR->GetComponent<Light>()->typeLight = Light::Type::Point;
	pointLightR->GetComponent<Light>()->lightColor = glm::vec3(1.0f, 0.0f, 1.0f);
	pointLightR->GetComponent<Light>()->intensive = 1.0f;
	pointLightR->AddComponent<LightMoving>();
	pointLightR->GetComponent<LightMoving>()->rigntPos = true;
	pointLightG->AddComponent<Render>();
	pointLightG->GetComponent<Render>()->mesh = &cube->meshes[0];
	pointLightG->GetComponent<Render>()->material = &cubeMaterial;
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
	std::cout << pointLight->GetComponent<Light>()->radius << " " << pointLightR->GetComponent<Light>()->radius << " " << pointLightG->GetComponent<Light>()->radius << "\n";
	directionalLight->AddComponent<Light>();
	directionalLight->GetComponent<Light>()->typeLight = Light::Type::Directional;
	directionalLight->GetComponent<Light>()->intensive = 0.3f;
	directionalLight->GetComponent<Light>()->shadow = true;
	directionalLight->GetComponent<Transform>()->position = glm::vec3(-1.0f, 6.0f, 0.5f);
	directionalLight->GetComponent<Transform>()->rotation = Quaternion::Euler(-45.0f, 45.0f, 45.0f);
	Plush_rei->AddComponent<Render>();
	Plush_rei->AddComponent<Rei>();
	Plush_rei->GetComponent<Render>()->mesh = &rei->meshes[0];
	Plush_rei->GetComponent<Render>()->material = &ToonMaterial;
	Plush_rei->GetComponent<Transform>()->position = glm::vec3(-2.0f, -0.2f, 0.5f);
	Plush_rei->GetComponent<Transform>()->UpdateMatrices();
	Plush_rei->AddComponent<Cube>();
	Plush_rei1->AddComponent<Render>();
	Plush_rei1->GetComponent<Render>()->mesh = &rei->meshes[0];
	Plush_rei1->GetComponent<Render>()->material = &material;
	Plush_rei1->GetComponent<Transform>()->position = glm::vec3(2.0f, -0.2f, 0.5f);
	Plush_rei1->GetComponent<Transform>()->UpdateMatrices();
	//Plush_rei1.AddComponent<Cube>();
	Plush_rei2->AddComponent<Render>();
	Plush_rei2->GetComponent<Render>()->mesh = &rei->meshes[0];
	Plush_rei2->GetComponent<Render>()->material = &material;
	Plush_rei2->GetComponent<Transform>()->position = glm::vec3(0.0f, -0.2f, 0.5f);
	Plush_rei2->GetComponent<Transform>()->UpdateMatrices();
	caveObject->AddComponent<Render>();
	caveObject->GetComponent<Render>()->mesh = &cave->meshes[0];
	caveObject->GetComponent<Render>()->material = &CaveMaterial;
	caveObject->GetComponent<Transform>()->position = glm::vec3(2.0f, -0.2f, 0.5f);
	caveObject->GetComponent<Transform>()->UpdateMatrices();
	//Plush_rei2.AddComponent<Cube>();
	camera->AddComponent<Camera>();
	camera->GetComponent<Transform>()->position = glm::vec3(2.0f, 2.0f, 7.0f);
	camera->GetComponent<Transform>()->rotation = Quaternion::Euler(0.0f, 0.0f, 0.0f);
	camera->GetComponent<Transform>()->UpdateMatrices();
	camera->AddComponent<PostProcess>();
	camera->GetComponent<PostProcess>()->exposure = 1.0f;
	camera->GetComponent<PostProcess>()->AutoExposure = true;
	camera->AddComponent<EditorCamera>();
	SceneManager::Get().GetActiveScene().HDR = true;

	rs_Init();
	auto renderContext = RenderContext{};
	//unsigned int frame = 0;
	while (!glfwWindowShouldClose(window.window))
	{
		//frame++;
		//if (frame > 10)
			//break;
		glfwPollEvents();
		if (!SceneManager::Get().GetActiveScene().AwakeAComponents.empty())
		{
			for (auto& component : SceneManager::Get().GetActiveScene().AwakeAComponents)
				component->Awake();
			SceneManager::Get().GetActiveScene().AwakeAComponents.clear();
		}
		if (!SceneManager::Get().GetActiveScene().StartAComponents.empty())
		{
			for (auto& component : SceneManager::Get().GetActiveScene().StartAComponents)
				component->Start();
			SceneManager::Get().GetActiveScene().StartAComponents.clear();
		}
		for (auto& component : SceneManager::Get().GetActiveScene().AComponents)
		{
			component->Update();
		}
		renderContext = SceneManager::Get().GetActiveScene().CollectRenderContext();
		renderContext.window = &window;
		if(window.editorGUI->selectedObject.selectedObjectRender)
			renderContext.selectedObject = window.editorGUI->selectedObject.selectedObjectRender;
		rs_Start();
		Time::UpdateTime();
		rs_Render(renderContext);
		window.editorGUI->RenderEditorParts();
		rs_End();
		rs_PostProcess(renderContext);
		rs_Present(renderContext);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		window.editorGUI->ShowInspectorWindow();
		window.editorGUI->ShowHierarchyWindow();
		window.editorGUI->RenderGizmo();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window.window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
int Application::close()
{
	glfwTerminate();
	glfwSetWindowShouldClose(window.window, true);
	return 0;
}