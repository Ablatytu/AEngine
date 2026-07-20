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
		static Cubemap* LoadCubemap(const std::string& path, unsigned int size);
		static HDRTexture* LoadHDRTexture(const std::string& textureName);
		static Material* CreateMaterial(Shader* shader);
		static const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures();
		static const std::unordered_map<std::string, std::unique_ptr<Cubemap>>& GetCubemaps();
};