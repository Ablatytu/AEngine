#include "Material.h"
#include "Shader.h"
#include <glad/glad.h>
#include "Texture.h"
#include "Cubemap.h"
#include "../Renderer/Buffer.h"

void Material::SetShader(Shader* newshader)
{
	shader = newshader;
	if (!shader)
		return;
	properties = std::make_unique<PropertiesWithUniforms>(shader->GetPathFragmentShader(), shader->GetShaderID());
	renderPath = static_cast<RenderPath>(properties->RenderPath);
	std::cout << static_cast<int>(renderPath) << std::endl;
#ifdef _DEBUG
	std::cout << "Create properties for material of shader. Pathshader: " << newshader->GetPathFragmentShader() << std::endl;
#endif
}
void Material::SetUBO(std::string nameUBO, std::string nameField, const void* data, uint32_t sizevalue)
{
	if (shader)
	{
		if (auto buf = UBOBuffer::existBuffer(nameUBO))
		{
			auto u = shader->GetUBOReflection()->GetUBOs().find(nameUBO);
			if (u != shader->GetUBOReflection()->GetUBOs().end())
			{
				auto uinfo = u->second.uniforms.find(nameField);
				if (uinfo != u->second.uniforms.end())
				{
					if (uinfo->second.sizeUniform >= sizevalue)
						UBOBuffer::ChangeInfoBuffer(nameUBO, sizevalue, data, uinfo->second.offset);
				}
			}
		}
	}
}
Shader* Material::GetShader()
{
	return shader;
}
Texture* Material::GetTexture(const std::string& name)
{
	auto t = textures.find(name);
	if (t != textures.end())
		return t->second;
	return nullptr;
}
void Material::Set(std::string name, float value)
{
	if (shader)
	{
		properties->SetValue<float>(name, value);
	}
}
void Material::Set(std::string name, int value)
{
	if (shader)
	{
		properties->SetValue<int>(name, value);
	}
}
void Material::Set(std::string name, bool value)
{
	if (shader)
	{
		properties->SetValue<bool>(name, value);
	}
}
void Material::Set(std::string name, float value0, float value1, float value2, float value3)
{
	if (shader)
	{
		properties->SetValue<GLSLParserMath::Vector4>(name, GLSLParserMath::Vector4{ value0, value1, value2, value3 });
	}
}
void Material::Set(std::string name, float value0, float value1, float value2)
{
	if (shader)
	{
		properties->SetValue<GLSLParserMath::Vector3>(name, GLSLParserMath::Vector3{ value0, value1, value2 });
	}
}
void Material::Set(std::string name, float value0, float value1)
{
	if (shader)
	{
		properties->SetValue<GLSLParserMath::Vector2>(name, GLSLParserMath::Vector2{ value0, value1 });
	}
}
void Material::Set(std::string name, glm::vec4 value)
{
	if (shader)
	{
		properties->SetValue<GLSLParserMath::Vector4>(name, GLSLParserMath::Vector4{ value.r, value.g, value.b, value.a });
	}
}
void Material::Set(std::string name, glm::vec3 value)
{
	if (shader)
	{
		properties->SetValue<GLSLParserMath::Vector3>(name, GLSLParserMath::Vector3{ value.r, value.g, value.b });
	}
}
void Material::Set(std::string name, glm::vec2 value)
{
	if (shader)
	{
		properties->SetValue<GLSLParserMath::Vector2>(name, GLSLParserMath::Vector2{ value.r, value.g });
	}
}
void Material::Set(std::string name, Texture* texture)
{
	if (shader)
	{
		if (texture)
			textures[name] = texture;
		else
			textures.erase(name);
	}
}
void Material::Set(std::string name, Cubemap* texture)
{
	if (shader)
	{
		cubemaps[name] = texture;
	}
}
PropertiesWithUniforms* Material::GetProperties()
{
	return properties.get();
}
void Material::UpdateMaterial()
{
	if (!shader) return;
	unsigned int slotTexture = 0;
	for (auto& field : properties->infoes)
	{
		if (field.second.location.location == -1) continue;
		switch (field.second.location.type)
		{
		case GL_FLOAT:
			if (auto t = properties->GetValue<float>(field.first))
			{
				shader->setFloat(field.first, *t);
			}
			break;
		case GL_INT:
			if (auto t = properties->GetValue<int>(field.first))
			{
				shader->setInt(field.first, *t);
			}
			break;
		case GL_BOOL:
			if (auto t = properties->GetValue<bool>(field.first))
			{
				shader->setBool(field.first, *t);
			}
			break;
		case GL_FLOAT_VEC4:
			if (auto t = properties->GetValue<GLSLParserMath::Vector4>(field.first))
			{
				shader->setVec4(field.first, t->x, t->y, t->z, t->w);
			}
			break;
		case GL_FLOAT_VEC3:
			if (auto t = properties->GetValue<GLSLParserMath::Vector3>(field.first))
			{
				shader->setVec3(field.first, t->x, t->y, t->z);
			}
			break;
		case GL_FLOAT_VEC2:
			if (auto t = properties->GetValue<GLSLParserMath::Vector2>(field.first))
			{
				shader->setVec2(field.first, t->x, t->y);
			}
			break;
		case GL_SAMPLER_2D:
			shader->setInt(field.first, slotTexture);
			if (textures.find(field.first) != textures.end())
			{
				glActiveTexture(GL_TEXTURE0 + slotTexture);
				glBindTexture(GL_TEXTURE_2D, textures.find(field.first)->second->texture);
			}
			else
			{
				auto it = properties->GetValue<std::string>(field.first);
				if (it)
				{
					if (*it == "blue")
					{
						glActiveTexture(GL_TEXTURE0 + slotTexture);
						glBindTexture(GL_TEXTURE_2D, GetBlueTexture()->texture);
					}
					else if (*it == "normal")
					{
						glActiveTexture(GL_TEXTURE0 + slotTexture);
						glBindTexture(GL_TEXTURE_2D, GetNormalTexture()->texture);
					}
					else if (*it == "black")
					{
						glActiveTexture(GL_TEXTURE0 + slotTexture);
						glBindTexture(GL_TEXTURE_2D, GetBlackTexture()->texture);
					}
					else
					{
						glActiveTexture(GL_TEXTURE0 + slotTexture);
						glBindTexture(GL_TEXTURE_2D, GetWhiteTexture()->texture);
					}
				}
				else
				{
					glActiveTexture(GL_TEXTURE0 + slotTexture);
					glBindTexture(GL_TEXTURE_2D, GetWhiteTexture()->texture);
				}
			}
			slotTexture++;
			break;
		case GL_SAMPLER_CUBE:
			shader->setInt(field.first, slotTexture);
			if (cubemaps.find(field.first) != cubemaps.end())
			{
				glActiveTexture(GL_TEXTURE0 + slotTexture);
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps.find(field.first)->second->textureID);
			}
			slotTexture++;
			break;
		}
	}
	lastTextureSlot = slotTexture;
}
unsigned int Material::GetLastTextureSlot() const
{
	return lastTextureSlot;
}
Texture* Material::GetWhiteTexture()
{
	static std::unique_ptr<Texture> WhiteTexture;
	if (!WhiteTexture)
	{
		WhiteTexture = std::make_unique<Texture>();
		WhiteTexture->CreateColorTexture("white");
	}
	return WhiteTexture.get();
}
Texture* Material::GetBlueTexture()
{
	static std::unique_ptr<Texture> BlueTexture;
	if (!BlueTexture)
	{
		BlueTexture = std::make_unique<Texture>();
		BlueTexture->CreateColorTexture("blue");
	}
	return BlueTexture.get();
}
Texture* Material::GetNormalTexture()
{
	static std::unique_ptr<Texture> NormalTexture;
	if (!NormalTexture)
	{
		NormalTexture = std::make_unique<Texture>();
		NormalTexture->CreateColorTexture("normal");
	}
	return NormalTexture.get();
}
Texture* Material::GetBlackTexture()
{
	static std::unique_ptr<Texture> BlackTexture;
	if (!BlackTexture)
	{
		BlackTexture = std::make_unique<Texture>();
		BlackTexture->CreateColorTexture("black");
	}
	return BlackTexture.get();
}
const Material::RenderPath& Material::GetRenderPath() const
{
	return renderPath;
}
Material::Material() = default;
Material::~Material() = default;