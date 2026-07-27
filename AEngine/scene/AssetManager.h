#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <optional>
#include "../core/AEngineAPI.h"

class Model;
class Shader;
class Cubemap;
class HDRTexture;
class Material;

enum class DataFormat;
enum class FilterType;
class Texture;


class AENGINE_API AssetManager
{
	public:
		static Model* LoadModel(const std::string& modelName);
		static Shader* LoadShader(const std::string& VertexName, const std::string& FragmentName, const std::optional<std::string>& pathGeometryShader = std::nullopt);
		static Texture* LoadTexture(const std::string& textureName, bool linear = false, std::optional<DataFormat> imageFormat = std::nullopt, std::optional<DataFormat> dataFormat = std::nullopt, std::vector<FilterType> filter1 = {}, std::vector<FilterType> filter2 = {});
		static HDRTexture* LoadHDRTexture(const std::string& textureName);
		static Material* LoadMaterial(const std::string& path);
		static Cubemap* LoadCubemap(const std::string& path);

		static Cubemap* CreateCubemap(const std::string& path, unsigned int size, bool CreateAsset = false);
		static Material* CreateMaterial(const std::string& path, Shader* shader, bool CreateAsset = false);

		static void SaveCubemap(Cubemap* cubemap, const std::string& path);
		static void SaveMaterial(Material* material, const std::string& path);

		static const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures();
		static const std::unordered_map<std::string, std::unique_ptr<Cubemap>>& GetCubemaps();
		static const std::unordered_map<std::string, std::unique_ptr<Shader>>& GetShaders();
};