#include "Shader.h"
#include "../core/ErrorManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Buffer.h"

const std::string& Shader::GetPathFragmentShader() const
{
	return pathFragmentShader;
}
const std::string& Shader::GetPathVertexShader() const
{
	return pathVertexShader;
}
const std::string& Shader::GetPathGeometryShader() const
{
	return pathGeometryShader;
}
unsigned int Shader::GetShaderID()
{
	return ID;
}
Shader::Shader(std::string pathShaderVertex, std::string pathShaderFragment, std::optional<std::string> pathShaderGeometry)
{
	std::ifstream fileVertex(pathShaderVertex);
	if (!fileVertex.is_open())
	{
		std::string error = std::string("Shader file not found: ") + pathShaderVertex;
		CriticalGameError(error.c_str());
		return;
	}
	int success = false;
	std::stringstream bufferVertex;
	bufferVertex << fileVertex.rdbuf();
	std::string VertexShaderData = bufferVertex.str();
	VertexShaderData = AGLSLShaderToGLSL::returnAGLSLShaderToGLSL(VertexShaderData);
	const char* VertexShader = VertexShaderData.c_str();
	unsigned int CreatedVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(CreatedVertexShader, 1, &VertexShader, NULL);
	glCompileShader(CreatedVertexShader);
	glGetShaderiv(CreatedVertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[1024];
		glGetShaderInfoLog(CreatedVertexShader, 1024, NULL, infoLog);
		std::cout << "Vertex shader error: " << infoLog << std::endl;
		CriticalGameError("Shader error");
		return;
	}
	std::ifstream fileFragment(pathShaderFragment);
	if (!fileFragment.is_open())
	{
		std::string error = std::string("Shader file not found: ") + pathShaderFragment;
		CriticalGameError(error.c_str());
		return;
	}
	std::stringstream bufferFragment;
	bufferFragment << fileFragment.rdbuf();
	std::string FragmentShaderData = bufferFragment.str();
	FragmentShaderData = AGLSLShaderToGLSL::returnAGLSLShaderToGLSL(FragmentShaderData);
	const char* FragmentShader = FragmentShaderData.c_str();
	unsigned int CreatedFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(CreatedFragmentShader, 1, &FragmentShader, NULL);
	glCompileShader(CreatedFragmentShader);
	glGetShaderiv(CreatedFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[1024];
		glGetShaderInfoLog(CreatedFragmentShader, 1024, NULL, infoLog);
		std::cout << "Fragment shader error: " << infoLog << std::endl;
		CriticalGameError("Shader error");
		return;
	}
	unsigned int CreatedGeometryShader;
	if (pathShaderGeometry)
	{
		std::ifstream fileGeometry(*pathShaderGeometry);
		if (!fileGeometry.is_open())
		{
			std::string error = std::string("Shader file not found: ") + *pathShaderGeometry;
			CriticalGameError(error.c_str());
			return;
		}
		std::stringstream sourceGeometryStream;
		sourceGeometryStream << fileGeometry.rdbuf();
		std::string GeometryShaderSource = sourceGeometryStream.str();
		GeometryShaderSource = AGLSLShaderToGLSL::returnAGLSLShaderToGLSL(GeometryShaderSource);
		const char* GeometryShader = GeometryShaderSource.c_str();
		CreatedGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(CreatedGeometryShader, 1, &GeometryShader, NULL);
		glCompileShader(CreatedGeometryShader);
		glGetShaderiv(CreatedGeometryShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infolog[1024];
			glGetShaderInfoLog(CreatedGeometryShader, 1024, NULL, infolog);
			std::cout << "Geometry Shader error: " << infolog << std::endl;
			CriticalGameError("Shader error");
			return;
		}
	}
	ID = glCreateProgram();
	glAttachShader(ID, CreatedVertexShader);
	glAttachShader(ID, CreatedFragmentShader);
	if (pathShaderGeometry)
		glAttachShader(ID, CreatedGeometryShader);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) 
	{
		char infoLog[1024];
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "Program link error:\n" << infoLog << std::endl;
		CriticalGameError("Shader error");
		return;
	}
	pathFragmentShader = pathShaderFragment;
	pathVertexShader = pathShaderVertex;
	pathGeometryShader = pathShaderGeometry? *pathShaderGeometry : "";
	UBOReflection = GLSLUBOUniforms(ID);
	for (auto& t : UBOReflection.GetUBOs())
	{
		if (auto buff = UBOBuffer::existBuffer(t.first))
		{
			if (buff->size != t.second.size)
				exit(0);
			unsigned int UBO = glGetUniformBlockIndex(ID, t.first.c_str());
			if (UBO != GL_INVALID_INDEX)
			{
				glUniformBlockBinding(ID, UBO, buff->binding);
			}
		}
		else
		{
			unsigned int UBO = glGetUniformBlockIndex(ID, t.first.c_str());
			if (UBO != GL_INVALID_INDEX)
			{
				glUniformBlockBinding(ID, UBO, UBOBuffer::GetLastUBO());
			}
			UBOBuffer::AddBuffer(t.first, t.second.size, false, UBOBuffer::GetLastUBO());
		}
	}

	glDeleteShader(CreatedVertexShader);
	glDeleteShader(CreatedFragmentShader);
	if (pathShaderGeometry)
		glDeleteShader(CreatedGeometryShader);
}
void Shader::Update()
{
	glUseProgram(ID);
}
void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
const GLSLUBOUniforms* Shader::GetUBOReflection() const
{
	return &UBOReflection;
}