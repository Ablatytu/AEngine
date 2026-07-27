#include <string>
#include <unordered_map>
#include "AssetManager.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Mesh.h"
#include "../APIs/ModelLoader.h"
#include "../Renderer/Cubemap.h"
#include "../Renderer/HDRTexture.h"
#include "../Renderer/Material.h"
#include "CubemapSerializer.h"
#include "MaterialSerializer.h"
#include <memory>
#include <optional>
#include <filesystem>

std::unordered_map<std::string, std::unique_ptr<Model>> models;
std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
std::unordered_map<std::string, std::unique_ptr<HDRTexture>> HDRtextures;
std::unordered_map<std::string, std::unique_ptr<Cubemap>> cubemaps;
std::unordered_map<std::string, std::unique_ptr<Material>> materials;

const std::unordered_map<std::string, std::unique_ptr<Texture>>& AssetManager::GetTextures()
{
	return textures;
}
const std::unordered_map<std::string, std::unique_ptr<Cubemap>>& AssetManager::GetCubemaps()
{
	return cubemaps;
}
const std::unordered_map<std::string, std::unique_ptr<Shader>>& AssetManager::GetShaders()
{
	return shaders;
}

Material* AssetManager::CreateMaterial(const std::string& path, Shader* shader, bool CreateAsset)
{
	auto finded = materials.find(path);
	if (finded != materials.end())
		return finded->second.get();
	std::unique_ptr<Material> newmaterial = std::make_unique<Material>();
	newmaterial->SetShader(shader);
	Material* result = newmaterial.get();
	result->path = path;
	if (CreateAsset)
	{
		result->hasFile = true;
		MaterialSerializer::Serialize(*result, path);
	}
	materials[path] = std::move(newmaterial);

	return result;
}
Model* AssetManager::LoadModel(const std::string& modelName)
{
	auto it = models.find(modelName);
	if (it != models.end())
		return it->second.get();
	std::unique_ptr<Model> model = std::make_unique<Model>(modelName);
	Model* returnModel = model.get();
	models[modelName] = std::move(model);
#ifdef _DEBUG
	std::cout << modelName << " has been sucñsessfully loaded" << std::endl;
#endif
	return returnModel;
}

Shader* AssetManager::LoadShader(const std::string& VertexName, const std::string& FragmentName, const std::optional<std::string>& GeometryShader)
{
	std::string nameshader = VertexName + "|" + FragmentName;
	if (GeometryShader)
		nameshader += "|" + *GeometryShader;
	auto it = shaders.find(nameshader);
	if (it != shaders.end())
		return it->second.get();
	std::unique_ptr<Shader> shader = std::make_unique<Shader>(VertexName, FragmentName, GeometryShader);
	Shader* returnShader = shader.get();
	shaders[nameshader] = std::move(shader);
#ifdef _DEBUG
	std::cout << nameshader << " has been sucñsessfully compiled" << std::endl;
#endif
	return returnShader;
}
Texture* AssetManager::LoadTexture(const std::string& textureName, bool linear, std::optional<Texture::DataFormat> imageFormat, std::optional<Texture::DataFormat> dataFormat, std::vector<Texture::FilterType> filter1, std::vector<Texture::FilterType> filter2)
{
	auto it = textures.find(textureName);
	if (it != textures.end())
		return it->second.get();
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	Texture* returnTexture = texture.get();
	texture->LoadTexture(textureName.c_str(), linear, imageFormat, dataFormat, filter1, filter2);
	textures[textureName] = std::move(texture);
#ifdef _DEBUG
	std::cout << textureName << " has been sucñsessfully loaded" << std::endl;
#endif
	return returnTexture;
}
Cubemap* AssetManager::CreateCubemap(const std::string& path, unsigned int size, bool CreateAsset)
{
	auto it = cubemaps.find(path);
	if (it != cubemaps.end())
		return it->second.get();
	std::unique_ptr<Cubemap> cubemap = std::make_unique<Cubemap>(size);
	cubemap->path = path;
	if (CreateAsset)
		CubemapSerializer::Serialize(*cubemap.get(), path);
	Cubemap* returnCubemap = cubemap.get();
	cubemaps[path] = std::move(cubemap);
#ifdef _DEBUG
	std::cout << path << " has been sucñsessfully loaded" << std::endl;
#endif
	return returnCubemap;
}
Cubemap* AssetManager::LoadCubemap(const std::string& path)
{
	if (std::filesystem::path(path).extension() != ".aasset")
	{
		std::cerr << "Invalid cubemap extension: " << path << '\n';
		return nullptr;
	}
	auto it = cubemaps.find(path);
	if (it != cubemaps.end())
		return it->second.get();
	std::unique_ptr<Cubemap> cubemap = CubemapSerializer::Deserialize(path);
	if (!cubemap)
		return nullptr;
	cubemap->path = path;
	Cubemap* returnCubemap = cubemap.get();
	cubemaps[path] = std::move(cubemap);
#ifdef _DEBUG
	std::cout << path << " has been sucñsessfully loaded" << std::endl;
#endif
	return returnCubemap;
}

Material* AssetManager::LoadMaterial(const std::string& path)
{
	if (std::filesystem::path(path).extension() != ".aasset")
	{
		std::cerr << "Invalid material extension: " << path << '\n';
		return nullptr;
	}
	auto it = materials.find(path);
	if (it != materials.end())
		return it->second.get();
	std::unique_ptr<Material> material = MaterialSerializer::Deserialize(path);
	if (!material)
		return nullptr;
	material->path = path;
	material->hasFile = true;
	Material* returnMaterial = material.get();
	materials[path] = std::move(material);
#ifdef _DEBUG
	std::cout << path << " has been sucñsessfully loaded" << std::endl;
#endif
	return returnMaterial;
}
void AssetManager::SaveMaterial(Material* material, const std::string& path)
{
	if (!material) return;

	material->hasFile = true;
	material->path = path;
	MaterialSerializer::Serialize(*material, path);
}

void AssetManager::SaveCubemap(Cubemap* cubemap, const std::string& path)
{
	if (!cubemap) return;
	
	cubemap->path = path;
	CubemapSerializer::Serialize(*cubemap, cubemap->path);
}
HDRTexture* AssetManager::LoadHDRTexture(const std::string& textureName)
{
	auto it = HDRtextures.find(textureName);
	if (it != HDRtextures.end())
		return it->second.get();
	std::unique_ptr<HDRTexture> texture = std::make_unique<HDRTexture>();
	HDRTexture* returnTexture = texture.get();
	texture->LoadTexture(textureName.c_str());
	HDRtextures[textureName] = std::move(texture);
#ifdef _DEBUG
	std::cout << textureName << " has been sucñsessfully loaded" << std::endl;
#endif
	return returnTexture;
}