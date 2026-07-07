#include <string>
#include <unordered_map>
#include "AssetManager.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Mesh.h"
#include "../APIs/ModelLoader.h"
#include "../Renderer/Cubemap.h"
#include "../Renderer/HDRTexture.h"
#include <memory>
#include <optional>

std::unordered_map<std::string, std::unique_ptr<Model>> models;
std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
std::unordered_map<std::string, std::unique_ptr<HDRTexture>> HDRtextures;
std::unordered_map<std::string, std::unique_ptr<Cubemap>> cubemaps;

const std::unordered_map<std::string, std::unique_ptr<Texture>>& AssetManager::GetTextures()
{
	return textures;
}
const std::unordered_map<std::string, std::unique_ptr<Cubemap>>& AssetManager::GetCubemaps()
{
	return cubemaps;
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
Cubemap* AssetManager::LoadCubemap(const std::string& path, unsigned int size)
{
	auto it = cubemaps.find(path);
	if (it != cubemaps.end())
		return it->second.get();
	std::unique_ptr<Cubemap> cubemap = std::make_unique<Cubemap>(size);
	Cubemap* returnCubemap = cubemap.get();
	cubemaps[path] = std::move(cubemap);
#ifdef _DEBUG
	std::cout << path << " has been sucñsessfully loaded" << std::endl;
#endif
	return returnCubemap;
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