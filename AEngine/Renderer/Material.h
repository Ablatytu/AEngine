#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../APIs/ParserGLSL.h"
#include "../core/AEngineAPI.h"

class Shader;
class Texture;
class Cubemap;


class AENGINE_API Material
{
public:
	enum class SurfaceType
	{
		Opaque = 0,
		Transparent = 1
	};
	enum class RenderPath
	{
		forward = 0,
		deferred = 1
	};
	Material();
	Material::SurfaceType surfaceType = Material::SurfaceType::Opaque;
	void SetShader(Shader* newshader);
	Shader* GetShader();
	Texture* GetTexture(const std::string& name);
	Cubemap* GetCubemap(const std::string& name);
	void SetUBO(std::string nameUBO, std::string nameField, const void* data, uint32_t sizevalue);
	void Set(std::string name, float value);
	void Set(std::string name, int value);
	void Set(std::string name, bool value);
	void Set(std::string name, float value0, float value1, float value2, float value3);
	void Set(std::string name, float value0, float value1, float value2);
	void Set(std::string name, float value0, float value1);
	void Set(std::string name, Texture* texture);
	void Set(std::string name, Cubemap* texture);
	void Set(std::string name, glm::vec4 value);
	void Set(std::string name, glm::vec3 value);
	void Set(std::string name, glm::vec2 value);
	const RenderPath& GetRenderPath() const;
	const std::string& GetVertexLayout() const;
	Texture* GetWhiteTexture();
	Texture* GetBlueTexture();
	Texture* GetNormalTexture();
	Texture* GetBlackTexture();
	Cubemap* GetBlackCubemap();
	unsigned int GetLastTextureSlot() const;
	template<typename type> const type* GetValue(std::string nameuniform)
	{
		if(auto t = properties->GetValue<type>(nameuniform))
			return t;
		return nullptr;
	}
	void UpdateMaterial();
	PropertiesWithUniforms* GetProperties();
	~Material();

	std::string path = "";
	bool hasFile = false;
private:
	RenderPath renderPath = RenderPath::forward;
	Shader* shader = nullptr;
	unsigned int lastTextureSlot = 0;
	std::string VertexLayout = "";
	std::unique_ptr<PropertiesWithUniforms> properties;
	std::unordered_map<std::string, Texture*> textures;
	std::unordered_map<std::string, Cubemap*> cubemaps;
};