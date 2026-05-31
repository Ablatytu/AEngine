#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <optional>

class Model;
class Shader;
class Texture;
class Cubemap;

class AssetManager
{
	public:
		static Model* LoadModel(const std::string& modelName);
		static Shader* LoadShader(const std::string& VertexName, const std::string& FragmentName, const std::optional<std::string>& pathGeometryShader = std::nullopt);
		static Texture* LoadTexture(const std::string& textureName, bool linear = false);
		static Cubemap* LoadCubemap(const std::string& path, unsigned int size);
		static const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures();
		static const std::unordered_map<std::string, std::unique_ptr<Cubemap>>& GetCubemaps();
};