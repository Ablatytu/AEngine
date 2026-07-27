#include "CubemapSerializer.h"
#include "../Renderer/Cubemap.h"
#include "AssetManager.h"
#include "../Renderer/Texture.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

bool CubemapSerializer::IsCubemap(const std::string& path)
{
	try
	{
		YAML::Node root = YAML::LoadFile(path);
		if (root["Type"] && root["Type"].as<std::string>() == "Cubemap")
			return true;
		return false;
	}
	catch (const YAML::Exception& error)
	{
		std::cerr << "Asset YAML error: " << error.what() << '\n';
		return false;
	}
}
bool CubemapSerializer::Serialize(const Cubemap& cubemap, const std::string& path)
{
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Type"; out << YAML::Value << "Cubemap";

	out << YAML::Key << "Size"; out << YAML::Value << cubemap.size;

	out << YAML::Key << "Textures";
	out << YAML::Value << YAML::BeginMap;

	auto findTextureFromCubemap = [](const Cubemap& cubemap, TypeTexture type)
		{
			auto it = cubemap.textures.find(type);
			if (it == cubemap.textures.end() || !it->second)
				return std::string("");

			return it->second->path;
		};

	out << YAML::Key << "Right"; out << YAML::Value << findTextureFromCubemap(cubemap, TypeTexture::right);
	out << YAML::Key << "Left"; out << YAML::Value << findTextureFromCubemap(cubemap, TypeTexture::left);
	out << YAML::Key << "Top"; out << YAML::Value << findTextureFromCubemap(cubemap, TypeTexture::top);
	out << YAML::Key << "Bottom"; out << YAML::Value << findTextureFromCubemap(cubemap, TypeTexture::bottom);
	out << YAML::Key << "Front"; out << YAML::Value << findTextureFromCubemap(cubemap, TypeTexture::front);
	out << YAML::Key << "Back"; out << YAML::Value << findTextureFromCubemap(cubemap, TypeTexture::back);

	out << YAML::EndMap;
	out << YAML::EndMap;

	std::ofstream file(path);

	if (!file)
	{
		std::cerr << "Cannot save cubemap: " << path << '\n';

		return false;
	}
	file << out.c_str();
	return file.good();
}
std::unique_ptr<Cubemap> CubemapSerializer::Deserialize(const std::string& path)
{
	try
	{
		YAML::Node root = YAML::LoadFile(path);

		if (!root["Type"] || root["Type"].as<std::string>() != "Cubemap")
			return nullptr;

		unsigned int size = root["Size"].as<unsigned int>();

		YAML::Node textures = root["Textures"];

		std::unique_ptr<Cubemap> cubemapLoaded = std::make_unique<Cubemap>(size);
		auto LoadFace = [&](std::string yamlname, TypeTexture type)
			{
				if (!textures[yamlname]) return;

				std::string texturePath = textures[yamlname].as<std::string>();

				if (texturePath.empty())
					return;
				Texture* texture = AssetManager::LoadTexture(texturePath);

				if (texture)
					cubemapLoaded->ChangeTexture(type, texture);
			};
		LoadFace("Right", TypeTexture::right);
		LoadFace("Left", TypeTexture::left);
		LoadFace("Top", TypeTexture::top);
		LoadFace("Bottom", TypeTexture::bottom);
		LoadFace("Front", TypeTexture::front);
		LoadFace("Back", TypeTexture::back);

		return cubemapLoaded;
	}
	catch (const YAML::Exception& error)
	{
		std::cerr << "Cubemap YAML error: "
			<< error.what() << '\n';

		return nullptr;
	}
}