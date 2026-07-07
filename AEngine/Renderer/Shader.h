#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../APIs/ParserGLSL.h"
#include <optional>
#include "../core/AEngineAPI.h"

class AENGINE_API Shader
{
public:
    Shader(std::string pathShaderFragment, std::string pathVertexShader, std::optional<std::string> pathGeometryShader = std::nullopt);
    std::string GetPathFragmentShader();
    unsigned int GetShaderID();
	void Update();
	void setBool(const std::string& name, bool value) const;

	void setInt(const std::string& name, int value) const;

	void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;

    void setVec2(const std::string& name, float x, float y) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;

    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec4(const std::string& name, const glm::vec4& value) const;

    void setVec4(const std::string& name, float x, float y, float z, float w) const;

    void setMat2(const std::string& name, const glm::mat2& mat) const;

    void setMat3(const std::string& name, const glm::mat3& mat) const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

    const GLSLUBOUniforms* GetUBOReflection() const;
private:
    unsigned int ID;
    std::string pathFragmentShader;
    GLSLUBOUniforms UBOReflection;
};
#endif 