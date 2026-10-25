#include "MaterialSerializer.h"
#include "../Renderer/Material.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Cubemap.h"
#include "../APIs/ParserGLSL.h"
#include "../scene/AssetManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ostream>
#include <yaml-cpp/yaml.h>

bool MaterialSerializer::Serialize(Material& material, const std::string& path)
{
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Type"; out << YAML::Value << "Material";

	out << YAML::Key << "Shader_vertex"; out << YAML::Value << material.GetShader()->GetPathVertexShader();
	out << YAML::Key << "Shader_fragment"; out << YAML::Value << material.GetShader()->GetPathFragmentShader();
	out << YAML::Key << "Shader_geometry"; out << YAML::Value << material.GetShader()->GetPathGeometryShader();

	out << YAML::Key << "Properties";
	out << YAML::Value << YAML::BeginMap;

	for (const auto& info : material.GetProperties()->infoes)
	{
		switch (info.second.info.typefield)
		{
			case PGLinfos::FieldType::Bool:
			{
				if (auto value = material.GetProperties()->GetValue<bool>(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << *value;
				}
				break;
			}
			case PGLinfos::FieldType::Int:
			{
				if (auto value = material.GetProperties()->GetValue<int>(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << *value;
				}
				break;
			}
			case PGLinfos::FieldType::Float:
			{
				if (auto value = material.GetProperties()->GetValue<float>(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << *value;
				}
				break;
			}
			case PGLinfos::FieldType::Sampler2D:
			{
				if (auto texture = material.GetTexture(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << texture->path;
				}
				else
				{
					out << YAML::Key << info.first; out << YAML::Value << "";
				}
				break;
			}
			case PGLinfos::FieldType::SamplerCube:
			{
				if (auto texture = material.GetCubemap(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << texture->path;
				}
				else
				{
					out << YAML::Key << info.first; out << YAML::Value << "";
				}
				break;
			}
			case PGLinfos::FieldType::Vector2:
			{
				if (auto vec2 = material.GetProperties()->GetValue<GLSLParserMath::Vector2>(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << YAML::Flow << YAML::BeginSeq; out << vec2->x << vec2->y; out << YAML::EndSeq;
				}
				break;
			}
			case PGLinfos::FieldType::Vector3:
			{
				if (auto vec3 = material.GetProperties()->GetValue<GLSLParserMath::Vector3>(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << YAML::Flow << YAML::BeginSeq; out << vec3->x << vec3->y << vec3->z; out << YAML::EndSeq;
				}
				break;
			}
			case PGLinfos::FieldType::Vector4:
			{
				if (auto vec4 = material.GetProperties()->GetValue<GLSLParserMath::Vector4>(info.first))
				{
					out << YAML::Key << info.first; out << YAML::Value << YAML::Flow << YAML::BeginSeq; out << vec4->x << vec4->y << vec4->z << vec4->w; out << YAML::EndSeq;
				}
				break;
			}
		default:
			break;
		}
	}

	out << YAML::EndMap;

	out << YAML::EndMap;

	std::ofstream savefile(path);

	if (!savefile)
	{
		std::cerr << "Cannot save material: " << path << '\n';

		return false;
	}

	savefile << out.c_str();

	return savefile.good();
}
std::unique_ptr<Material> MaterialSerializer::Deserialize(const std::string& path)
{
	try
	{
		YAML::Node root = YAML::LoadFile(path);

		if (!root["Type"] || root["Type"].as<std::string>() != "Material")
			return nullptr;

		if (!root["Shader_vertex"]) return nullptr;

		std::string VertexShader = root["Shader_vertex"].as<std::string>();

		if (!root["Shader_fragment"]) return nullptr;

		std::string FragmentShader = root["Shader_fragment"].as<std::string>();

		if (!root["Shader_geometry"]) return nullptr;

		std::string GeometryShader = root["Shader_geometry"].as<std::string>();

		std::unique_ptr<Material> DeserializeMaterial = std::make_unique<Material>();

		if (GeometryShader == "")
			DeserializeMaterial->SetShader(AssetManager::LoadShader(VertexShader, FragmentShader));
		else
			DeserializeMaterial->SetShader(AssetManager::LoadShader(VertexShader, FragmentShader, GeometryShader));

		YAML::Node properties = root["Properties"];

		if (properties && properties.IsMap())
		{
			for (const auto& property : properties)
			{
				std::string name = property.first.as<std::string>();
				YAML::Node value = property.second;

				PGLinfos::FieldType type = DeserializeMaterial->GetProperties()->GetType(name);

				switch (type)
				{
				case PGLinfos::FieldType::Float:
				{
					DeserializeMaterial->Set(name, value.as<float>());
					break;
				}
				case PGLinfos::FieldType::Vector3:
				{
					if (value.IsSequence() && value.size() == 3)
						DeserializeMaterial->Set(name, glm::vec3(value[0].as<float>(), value[1].as<float>(), value[2].as<float>()));
					break;
				}
				case PGLinfos::FieldType::Sampler2D:
				{
					std::string TexturePath = value.as<std::string>();
					if(!TexturePath.empty())
						DeserializeMaterial->Set(name, AssetManager::LoadTexture(value.as<std::string>()));
					else
						DeserializeMaterial->Set(name, (Texture*)nullptr);
					break;
				}
				case PGLinfos::FieldType::SamplerCube:
				{
					std::string CubemapPath = value.as<std::string>();
					if (!CubemapPath.empty())
						DeserializeMaterial->Set(name, AssetManager::LoadCubemap(value.as<std::string>()));
					else
						DeserializeMaterial->Set(name, (Cubemap*)nullptr);
					break;
				}
				case PGLinfos::FieldType::Bool:
				{
					DeserializeMaterial->Set(name, value.as<bool>());
					break;
				}
				case PGLinfos::FieldType::Int:
				{
					DeserializeMaterial->Set(name, value.as<int>());
					break;
				}
				case PGLinfos::FieldType::Vector2:
				{
					if (value.IsSequence() && value.size() == 2)
						DeserializeMaterial->Set(name, glm::vec2(value[0].as<float>(), value[1].as<float>()));
					break;
				}
				case PGLinfos::FieldType::Vector4:
				{
					if (value.IsSequence() && value.size() == 4)
						DeserializeMaterial->Set(name, glm::vec4(value[0].as<float>(), value[1].as<float>(), value[2].as<float>(), value[3].as<float>()));
					break;
				}
				case PGLinfos::FieldType::Unknown:
				{

					break;
				}
				}
			}
		}
		return DeserializeMaterial;
	}
	catch (const YAML::Exception& error)
	{
		std::cout << "ERRORMATERIALYAMLEXCEPTION\n";
		return nullptr;
	}
}
bool MaterialSerializer::IsMaterial(const std::string& path)
{
	try
	{
		YAML::Node root = YAML::LoadFile(path);
		if (root["Type"] && root["Type"].as<std::string>() == "Material")
			return true;
		return false;
	}
	catch (const YAML::Exception& error)
	{
		std::cout << "ERRORCHECKTYPEMATERIALYAMLEXCEPTION\n";
		return false;
	}
}