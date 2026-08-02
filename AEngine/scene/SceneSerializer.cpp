#include "SceneSerializer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Skybox.h"
#include "../Renderer/Cubemap.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/GameObject.h"
#include "../components/cube.h"
#include "../components/LightMoving.h"
#include "../components/AComponent.h"
#include "../components/Rei.h"
#include "../APIs/ModelLoader.h"
#include "AssetManager.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <filesystem>

bool SceneSerializer::SerializeEmpty(const std::string& path)
{

	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "SceneSettings";
	out << YAML::Value << YAML::BeginMap;

	out << YAML::Key << "Skybox"; out << YAML::Value << "";

	out << YAML::EndMap;

	out << YAML::Key << "GameObjects";
	out << YAML::Value << YAML::BeginSeq;
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream savefile(path);


	if (!savefile)
	{
		std::cerr << "Cannot save scene: " << path << '\n';

		return false;
	}

	savefile << out.c_str();

	return savefile.good();

}
bool SceneSerializer::Serialize(Scene& scene, const std::string& path)
{
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "SceneSettings";
	out << YAML::Value << YAML::BeginMap;
	
	out << YAML::Key << "Skybox"; out << YAML::Value << scene.skyBox->GetSkyboxCubemap()->path;

	out << YAML::EndMap;

	out << YAML::Key << "GameObjects"; 
	out << YAML::Value << YAML::BeginSeq;

	for (auto& gameobject : scene.objects)
	{

		if (gameobject->name == "!##@@AENGINECAMERAEDITOR@@##!") continue;

		out << YAML::BeginMap;
		out << YAML::Key << "Name"; out << YAML::Value << gameobject->name;
		out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;
		for (auto& component : gameobject->components)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "ComponentName"; out << YAML::Value << component->GetComponentName();
			component->fields.clear();
			component->OnInspector();
			out << YAML::Key << "Fields"; out << YAML::Value << YAML::BeginMap;
			for (auto& field : component->fields)
			{
				FieldType type = field.type;
				out << YAML::Key << field.name;
				switch (type)
				{
					case FieldType::BOOL:
					{
						out << YAML::Value << *static_cast<bool*>(field.ptr);
						break;
					}
					case FieldType::FLOAT:
					{
						out << YAML::Value << *static_cast<float*>(field.ptr);
						break;
					}
					case FieldType::VEC3:
					{
						glm::vec3 vec3 = *static_cast<glm::vec3*>(field.ptr);
						out << YAML::Value << YAML::Flow << YAML::BeginSeq; out << vec3.r << vec3.g << vec3.b; out << YAML::EndSeq;
						break;
					}
					case FieldType::VEC4:
					{
						glm::vec4 vec4 = *static_cast<glm::vec4*>(field.ptr);
						out << YAML::Value << YAML::Flow << YAML::BeginSeq; out << vec4.r << vec4.g << vec4.b << vec4.a; out << YAML::EndSeq;
						break;
					}
					case FieldType::INT:
					{
						out << YAML::Value << *static_cast<int*>(field.ptr);
						break;
					}
					case FieldType::QUAT:
					{
						glm::quat* valueq = static_cast<glm::quat*>(field.ptr);
						glm::vec3 euler = glm::degrees(glm::eulerAngles(*valueq));
						out << YAML::Value << YAML::Flow << YAML::BeginSeq; out << euler.r << euler.g << euler.b;  out << YAML::EndSeq;
						break;
					}
					case FieldType::MATERIAL:
					{
						Material* material = *static_cast<Material**>(field.ptr);
						if(material)
							out << YAML::Value << material->path;
						else
							out << YAML::Value << "";
						break;
					}
					case FieldType::MESH:
					{
						Mesh* mesh = *static_cast<Mesh**>(field.ptr);
						if (mesh)
							out << YAML::Value << mesh->basepathModel + "@@##AENGINENUMBERMESH##@@" + std::to_string(mesh->IDMesh);
						else
							out << YAML::Value << "";
						break;
					}
					case FieldType::ENUM:
					{
						out << YAML::Value << *static_cast<int*>(field.ptr);
						break;
					}
				}
			}
			out << YAML::EndMap;
			out << YAML::EndMap;
			
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream savefile(path);

	if (!savefile)
	{
		std::cerr << "Cannot save scene: " << path << '\n';

		return false;
	}

	savefile << out.c_str();

	return savefile.good();

}
std::unique_ptr<Scene> SceneSerializer::Deserialize(const std::string& path)
{
	YAML::Node root = YAML::LoadFile(path);

	std::unique_ptr<Scene> newScene = std::make_unique<Scene>();

	Scene* oldActiveScene = SceneManager::Get().SwapActiveScene(newScene.get());
	if (!root["SceneSettings"]) return nullptr;
	YAML::Node sceneSettings = root["SceneSettings"];
	if (sceneSettings && sceneSettings["Skybox"])
	{
		auto skyboxName = sceneSettings["Skybox"].as<std::string>();
		if (!skyboxName.empty())
		{
			std::filesystem::path skyboxPath(skyboxName);
			if (skyboxPath.extension().generic_string() == ".hdr")
			{
				HDRTexture* hdrSkybox = AssetManager::LoadHDRTexture(skyboxName);
				if (hdrSkybox)
					newScene->SetSkyBox(hdrSkybox);
			}
			else if (skyboxPath.extension().generic_string() == ".acubemap")
			{
				Cubemap* cubSkybox = AssetManager::LoadCubemap(skyboxName);
				if (cubSkybox)
					newScene->SetSkyBox(cubSkybox);
			}
		}
	}
	AComponent* NewComponent = nullptr;
	YAML::Node gameobjectsNode = root["GameObjects"];
	if (gameobjectsNode && gameobjectsNode.IsSequence())
	{
		for (auto gameobject : gameobjectsNode)
		{
			std::string name = gameobject["Name"].as<std::string>();
#ifdef _DEBUG
			std::cout << "Loading gameobject: " << name << "\n";
#endif 
			GameObject* newGameObject = newScene->CreateGameObject();
			newGameObject->name = name;
			auto componentsNode = gameobject["Components"];
			if (componentsNode && componentsNode.IsSequence())
			{
				for (auto component : componentsNode)
				{
					std::string nameComponent = component["ComponentName"].as<std::string>();
#ifdef _DEBUG
					std::cout << "Loading component: " << nameComponent << "\n";
#endif 
					if (nameComponent == "Transform")
					{
						NewComponent = newGameObject->GetComponent<Transform>();
					}
					else if (nameComponent == "Render")
					{
						NewComponent = newGameObject->AddComponent<Render>();
					}
					else if (nameComponent == "Camera")
					{
						NewComponent = newGameObject->AddComponent<Camera>();
					}
					else if (nameComponent == "Cube")
					{
						NewComponent = newGameObject->AddComponent<Cube>();
					}
					else if (nameComponent == "LightMoving")
					{
						NewComponent = newGameObject->AddComponent<LightMoving>();
					}
					else if (nameComponent == "Rei")
					{
						NewComponent = newGameObject->AddComponent<Rei>();
					}
					else if (nameComponent == "Light")
					{
						NewComponent = newGameObject->AddComponent<Light>();
					}
					else
						continue;
					NewComponent->OnInspector();
					auto fieldsNode = component["Fields"];

					if (fieldsNode && fieldsNode.IsMap())
					{
						for (auto field : fieldsNode)
						{
							std::string fieldName = field.first.as<std::string>();
							YAML::Node value = field.second;
#ifdef _DEBUG
							std::cout << "field name: " << fieldName << "\n";
#endif
							for (FieldInfo& infoComponent : NewComponent->fields)
							{
								if (std::string(infoComponent.name) == fieldName)
								{
									FieldType type = infoComponent.type;
									switch (type)
									{
									case FieldType::BOOL:
									{
										bool* valueb = static_cast<bool*>(infoComponent.ptr);
#ifdef _DEBUG
										if(valueb)
											std::cout << "field value: " << *valueb << "\n";
#endif
										if (valueb)
											*valueb = value.as<bool>();
										break;
									}
									case FieldType::FLOAT:
									{
										float* valuef = static_cast<float*>(infoComponent.ptr);
#ifdef _DEBUG
										if (valuef)
											std::cout << "field value: " << *valuef << "\n";
#endif
										if (valuef)
											*valuef = value.as<float>();
										break;
									}
									case FieldType::VEC3:
									{
										glm::vec3* value3 = static_cast<glm::vec3*>(infoComponent.ptr);
										if (value3 && value.IsSequence())
											*value3 = glm::vec3(value[0].as<float>(), value[1].as<float>(), value[2].as<float>());
#ifdef _DEBUG
										if (value3)
											std::cout << "field value: " << value3->r << " " << value3->g << " " << value3->b << " " << "\n";
#endif
										break;
									}
									case FieldType::VEC4:
									{
										glm::vec4* value4 = static_cast<glm::vec4*>(infoComponent.ptr);
#ifdef _DEBUG
										if (value4)
											std::cout << "field value: " << value4->r << " " << value4->g << " " << value4->b << " " << value4->a << "\n";
#endif
										if (value4 && value.IsSequence())
											*value4 = glm::vec4(value[0].as<float>(), value[1].as<float>(), value[2].as<float>(), value[3].as<float>());
										break;
									}
									case FieldType::INT:
									{
										int* valuei = static_cast<int*>(infoComponent.ptr);
#ifdef _DEBUG
										if (valuei)
											std::cout << "field value: " << *valuei << "\n";
#endif _DEBUG
										if (valuei)
											*valuei = value.as<int>();
										break;
									}
									case FieldType::QUAT:
									{
										glm::quat* valueq = static_cast<glm::quat*>(infoComponent.ptr);
										if (valueq && value.IsSequence())
											*valueq = glm::quat(glm::radians(glm::vec3(value[0].as<float>(), value[1].as<float>(), value[2].as<float>())));
#ifdef _DEBUG
										if (valueq)
										{
											glm::vec3 degrees = glm::degrees(glm::eulerAngles(*valueq));
											std::cout << "field value: " << degrees.r << " " << degrees.g << " " << degrees.b << " " << "\n";
										}
#endif
										break;
									}
									case FieldType::MATERIAL:
									{
										Material** valueml = static_cast<Material**>(infoComponent.ptr);
										if (valueml && !value.as<std::string>().empty())
											*valueml = AssetManager::LoadMaterial(value.as<std::string>());
										break;
									}
									case FieldType::MESH:
									{
										Mesh** valuemh = static_cast<Mesh**>(infoComponent.ptr);
										if (valuemh && !value.as<std::string>().empty())
										{
											std::string nameMesh = value.as<std::string>();
											size_t NumberMesh = nameMesh.rfind("@@##AENGINENUMBERMESH##@@");
											std::string numberMeshstr = nameMesh.substr(NumberMesh + std::string("@@##AENGINENUMBERMESH##@@").length());
											unsigned int numberMeshint = std::stoi(numberMeshstr);
											std::string cleanPath = nameMesh.substr(0, NumberMesh);
											*valuemh = &AssetManager::LoadModel(cleanPath)->meshes[numberMeshint];
										}
										break;
									}
									case FieldType::ENUM:
									{
										int* valuee = static_cast<int*>(infoComponent.ptr);
										if (valuee)
											*valuee = value.as<int>();
										break;
									}
									}
								}

							}
						}
						NewComponent->fields.clear();
						NewComponent = nullptr;

					}
				}

			}
		}
		SceneManager::Get().SwapActiveScene(oldActiveScene);

		return newScene;
	}
}