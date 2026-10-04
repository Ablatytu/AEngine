#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <memory>
#include <glad/glad.h>
#include <map>
#include "../core/AEngineAPI.h"
namespace GLSLParserMath
{
	struct Vector3
	{
		union
		{
			struct { float x, y, z; };
			struct { float r, g, b; };
			float data[3];
		};
		static Vector3 float3_to_vector3(const float* values)
		{
			return GLSLParserMath::Vector3{ *(values), *(values + 1), *(values + 2) };
		}
	};
	struct Vector4
	{
		union
		{
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
			float data[4];
		};
		static Vector4 float4_to_vector4(const float* values)
		{
			return GLSLParserMath::Vector4{ *(values), *(values + 1), *(values + 2), *(values + 3) };
		}
	};
	struct Vector2
	{
		union
		{
			struct { float x, y; };
			struct { float u, v; };
			float data[2];
		};
		static Vector2 float2_to_vector2(const float* values)
		{
			return GLSLParserMath::Vector2{ *(values), *(values + 1) };
		}
	};
}
namespace PGLinfos
{
	enum class FieldType
	{
		Float,
		Vector3,
		Sampler2D,
		SamplerCube,
		Bool,
		Int,
		Vector2,
		Vector4,
		Unknown
	};
	struct FieldInfo
	{
		FieldType typefield;
		std::variant<std::monostate, int, float, GLSLParserMath::Vector3, GLSLParserMath::Vector4, GLSLParserMath::Vector2, bool, std::string> value;
		bool isColor = false;
		bool useRange = false;
		float minrange = 0.0f;
		float maxrange = 0.0f;
		bool RGB = false;
	};
	struct UniformGPU
	{
		GLenum type;
		GLint location;
		GLint size;
	};
	struct UniformUBOGPU
	{
		uint32_t sizeUniform;
		GLenum type;
		uint32_t offset;
	};
	struct UBOReflection
	{
		uint32_t size;
		std::unordered_map<std::string, UniformUBOGPU> uniforms;
	};
	struct ShaderInfo
	{
		FieldInfo info;
		UniformGPU location;
	};
}
class AENGINE_API PropertiesWithUniforms
{
public:
	std::vector<std::string> propertiesNames;
	unsigned int RenderPath = 0;
	std::string VertexLayout = "";
	std::unordered_map<std::string, PGLinfos::ShaderInfo> infoes;
	PropertiesWithUniforms(std::string path, unsigned int shader);

	const PGLinfos::ShaderInfo* GetInfo(std::string name);

	const PGLinfos::FieldType GetType(std::string name);

	template<typename type> void SetValue(const std::string& name, const type& newValue)
	{
		auto it = infoes.find(name);
		if (it != infoes.end())
		{
			if (std::holds_alternative<type>(it->second.info.value))
			{
				it->second.info.value = newValue;
			}
		}
	}
	template<typename type> const type* GetValue(std::string name) const
	{
		auto it = infoes.find(name);
		if (it != infoes.end())
		{
			if (std::holds_alternative<type>(it->second.info.value))
				return &std::get<type>(it->second.info.value);
		}
		return nullptr;
	}
private:
	GLenum CPUTypeToGPU(const PGLinfos::FieldType& type);
	PGLinfos::FieldType GPUTypeToCPU(const GLenum& type);
};
class GLSLInfo
{
public:
	bool valid = false;
	unsigned int RenderPath = 0;
	std::string VertexLayout = "";
	const std::unordered_map<std::string, PGLinfos::FieldInfo>& returnAllFields() const;
	void AddField(const std::string& name, const PGLinfos::FieldInfo& info);
	std::vector<std::string>& returnAllFieldsName();
	const PGLinfos::FieldInfo* returnField(std::string name);
private:
	std::unordered_map<std::string, PGLinfos::FieldInfo> fields;
	std::vector<std::string> fieldsNames;
};
class GLSLUniforms
{
public:
	GLSLUniforms(unsigned int shaderID);
	const std::unordered_map<std::string, PGLinfos::UniformGPU>& GetUniforms()  const;
private:
	std::unordered_map<std::string, PGLinfos::UniformGPU> uniforms;
};
class GLSLUBOUniforms
{
public:
	GLSLUBOUniforms() = default;
	GLSLUBOUniforms(unsigned int shaderID);
	const std::map<std::string, PGLinfos::UBOReflection>& GetUBOs() const;
private:
	std::map<std::string, PGLinfos::UBOReflection> UBOSGPU;
};
class AGLSLShaderToGLSL
{
public:
	static std::string returnAGLSLShaderToGLSL(const std::string& AGLSLFileSource);
private:
	static std::string returnAGLSLShaderToGLSLFunction(const std::string& AGLSLFileSource, bool isheader);
	static std::vector<std::string> includedFiles;
};
class ParserGLSL
{
public:
	static GLSLInfo Parser(std::string path);
	static bool emptyline(std::string line, size_t start, size_t end);
private:
	static std::vector<float> returnVectorDirty(int size, std::string argument, size_t argumnet_start, size_t argument_end);
	static std::vector<float> returnVectorClear(int size, std::string argument);
};