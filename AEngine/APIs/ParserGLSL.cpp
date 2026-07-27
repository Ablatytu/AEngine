#include "ParserGLSL.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include <algorithm>

GLenum PropertiesWithUniforms::CPUTypeToGPU(const PGLinfos::FieldType& type)
{
	switch (type)
	{
	case PGLinfos::FieldType::Float:
		return GL_FLOAT;
		break;
	case PGLinfos::FieldType::Int:
		return GL_INT;
		break;
	case PGLinfos::FieldType::Vector2:
		return GL_FLOAT_VEC2;
		break;
	case PGLinfos::FieldType::Vector3:
		return GL_FLOAT_VEC3;
		break;
	case PGLinfos::FieldType::Vector4:
		return GL_FLOAT_VEC4;
		break;
	case PGLinfos::FieldType::Sampler2D:
		return GL_SAMPLER_2D;
		break;
	case PGLinfos::FieldType::Bool:
		return GL_BOOL;
		break;
	case PGLinfos::FieldType::SamplerCube:
		return GL_SAMPLER_CUBE;
		break;
	}
	throw std::runtime_error("Unsupported FieldType");
}
PGLinfos::FieldType PropertiesWithUniforms::GPUTypeToCPU(const GLenum& type)
{
	switch (type)
	{
	case GL_FLOAT:
		return PGLinfos::FieldType::Float;
		break;
	case GL_FLOAT_VEC2:
		return PGLinfos::FieldType::Vector2;
		break;
	case GL_FLOAT_VEC3:
		return PGLinfos::FieldType::Vector3;
		break;
	case GL_FLOAT_VEC4:
		return PGLinfos::FieldType::Vector4;
		break;
	case GL_SAMPLER_2D:
		return PGLinfos::FieldType::Sampler2D;
		break;
	case GL_BOOL:
		return PGLinfos::FieldType::Bool;
		break;
	case GL_INT:
		return PGLinfos::FieldType::Int;
		break;
	case GL_SAMPLER_CUBE:
		return PGLinfos::FieldType::SamplerCube;
		break;
	default:
		return PGLinfos::FieldType::Unknown;
		break;
	}
	std::cout << "unknown type" << std::endl;
	throw std::runtime_error("Unsupported GLType");
}
const PGLinfos::ShaderInfo* PropertiesWithUniforms::GetInfo(std::string name)
{
	auto finded = infoes.find(name);
	if(finded != infoes.end())
		return &finded->second;
	return nullptr;
}
const PGLinfos::FieldType PropertiesWithUniforms::GetType(std::string name)
{
	auto finded = infoes.find(name);
	if (finded != infoes.end())
		return finded->second.info.typefield;
	return PGLinfos::FieldType::Unknown;
}
PropertiesWithUniforms::PropertiesWithUniforms(std::string path, unsigned int shaderID)
{
	GLSLInfo infoProperties = ParserGLSL::Parser(path);
	RenderPath = infoProperties.RenderPath;
	GLSLUniforms infoGPU(shaderID);
	auto& namesuniforms = infoProperties.returnAllFieldsName();
	propertiesNames = namesuniforms;
	for (auto& field : infoGPU.GetUniforms())
	{
		auto t = infoProperties.returnAllFields().find(field.first);
		if (t != infoProperties.returnAllFields().end())
		{
			GLenum typeUniformCPU = CPUTypeToGPU(t->second.typefield);
			if (typeUniformCPU != field.second.type)
			{
				std::cout << "TypeUniformCpu != TypeUniformGPU. Default parameter value ignored" << std::endl;
				continue;
			}
			infoes[field.first] = PGLinfos::ShaderInfo{ t->second,  field.second };
		}
		else
		{
			PGLinfos::FieldInfo info{};
			info.isColor = false;
			info.useRange = false;
			info.typefield = GPUTypeToCPU(field.second.type);
			switch (info.typefield)
			{
			case PGLinfos::FieldType::Float:
				info.value = 0.0f;
				break;

			case PGLinfos::FieldType::Int:
				info.value = 0;
				break;

			case PGLinfos::FieldType::Bool:
				info.value = false;
				break;

			case PGLinfos::FieldType::Vector2:
				info.value = GLSLParserMath::Vector2{ 0.0f, 0.0f };
				break;

			case PGLinfos::FieldType::Vector3:
				info.value = GLSLParserMath::Vector3{ 0.0f, 0.0f, 0.0f };
				break;

			case PGLinfos::FieldType::Vector4:
				info.value = GLSLParserMath::Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
				break;

			case PGLinfos::FieldType::Sampler2D:
				info.value = std::string("white");
				break;

			case PGLinfos::FieldType::SamplerCube:
				info.value = std::string("white");
				break;
			case PGLinfos::FieldType::Unknown:
				continue;
				break;
			}
			infoes[field.first] = PGLinfos::ShaderInfo{ info, field.second};
		}
	}
}
const std::unordered_map<std::string, PGLinfos::UniformGPU>& GLSLUniforms::GetUniforms() const
{
	return uniforms;
}
GLSLUniforms::GLSLUniforms(unsigned int shaderID)
{
	GLint uniformcounts = 0;
	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &uniformcounts);
	for (GLint i = 0; i < uniformcounts; ++i)
	{
		PGLinfos::UniformGPU newuniform{};
		GLchar name[256];
		GLenum type;
		GLint size;
		glGetActiveUniform(shaderID, i, sizeof(name), nullptr, &size, &type, name);

		GLint location = glGetUniformLocation(shaderID, name);
		if (location == -1)
			continue;
		newuniform.type = type;
		newuniform.location = location;
		newuniform.size = size;
		std::string uniformName = name;
		uniforms[uniformName] = newuniform;
	}
}
GLSLUBOUniforms::GLSLUBOUniforms(unsigned int shaderID)
{
	GLint UBOCount = 0;
	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_BLOCKS, &UBOCount);
	for (GLint numberUBO = 0; numberUBO < UBOCount; numberUBO++)
	{
		PGLinfos::UBOReflection newUBO{};
		GLchar blockName[256];
		GLsizei nameLength;
		glGetActiveUniformBlockName(shaderID, numberUBO, sizeof(blockName), &nameLength, blockName);
		GLsizei UBOsize;
		glGetActiveUniformBlockiv(shaderID, numberUBO, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);
		GLint uniformCount;
		glGetActiveUniformBlockiv(shaderID, numberUBO, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);
		std::vector<GLint> indices(uniformCount);
		glGetActiveUniformBlockiv(shaderID, numberUBO, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices.data());
		std::vector<GLint> offsets(uniformCount);
		glGetActiveUniformsiv(shaderID, uniformCount, (GLuint*)indices.data(), GL_UNIFORM_OFFSET, offsets.data());
		for (int i = 0; i < uniformCount; i++)
		{
			PGLinfos::UniformUBOGPU newUnifromUBO{};
			GLchar name[256];
			GLsizei length;
			GLint size;
			GLenum type;
			glGetActiveUniform(shaderID, indices[i], sizeof(name), &length, &size, &type, name);
			newUnifromUBO.offset = offsets[i];
			newUnifromUBO.type = type;
			if (i != uniformCount - 1)
				newUnifromUBO.sizeUniform = offsets[i + 1] - offsets[i];
			else
				newUnifromUBO.sizeUniform = UBOsize - offsets[i];
			newUBO.uniforms[name] = newUnifromUBO;
		}
		newUBO.size = UBOsize;
		UBOSGPU[std::string(blockName, nameLength)] = newUBO;
	}
}
const std::map<std::string, PGLinfos::UBOReflection>& GLSLUBOUniforms::GetUBOs() const
{
	return UBOSGPU;
}
const PGLinfos::FieldInfo* GLSLInfo::returnField(std::string name)
{
	auto it = fields.find(name);
	if (it != fields.end())
	{
		return &it->second;
	}
	return nullptr;
}
bool ParserGLSL::emptyline(std::string line, size_t start, size_t end)
{
	if (start > end) throw std::runtime_error("error emptyLine. StartLine > EndLine");
	std::string newline = line.substr(start, end - start);
	newline.erase(std::remove_if(newline.begin(), newline.end(), [](char c) {return std::isspace((unsigned char)c); }), newline.end());
	return newline.empty();
}
std::vector<float> ParserGLSL::returnVectorDirty(int size, std::string argument, size_t argumnet_start, size_t argument_end)
{
	std::vector<float> numbers;
	size_t start_argument = argument.find("(", argumnet_start);
	if (start_argument == std::string::npos) return numbers;
	size_t end_argument = argument.find(")", start_argument);
	if (start_argument != std::string::npos && end_argument != std::string::npos)
	{
		if (end_argument > argument_end) return numbers;
		std::string info_argument = argument.substr(start_argument + 1, end_argument - start_argument - 1);
		std::stringstream vector_stream(info_argument);
		std::string number;
		for (int i = 0; i < size; i++)
		{
			if (std::getline(vector_stream, number, ','))
			{
				try
				{
					numbers.push_back(std::stof(number));
				}
				catch (const std::invalid_argument& e)
				{
					std::cout << "Error value type. IT IS NOT A FLOAT" << std::endl;
					numbers.push_back(0.0f);
				}
				catch (const std::out_of_range& e)
				{
					std::cout << "Error value type. IT OUT OF RANGE" << std::endl;
					numbers.push_back(0.0f);
				}
			}
			else
				numbers.push_back(0.0f);
		}
	}
	return numbers;
}
std::vector<float> ParserGLSL::returnVectorClear(int size, std::string argument)
{
	std::vector<float> numbers;
	size_t start_argument = argument.find("(");
	if (start_argument == std::string::npos) return numbers;
	size_t end_argument = argument.find(")", start_argument);
	if (start_argument != std::string::npos && end_argument != std::string::npos)
	{
		std::string info_argument = argument.substr(start_argument + 1, end_argument - start_argument - 1);
		info_argument.erase(std::remove_if(info_argument.begin(), info_argument.end(), [](char c) {return std::isspace((unsigned char)c); }), info_argument.end());
		std::stringstream vector_stream(info_argument);
		std::string number;
		for (int i = 0; i < size; i++)
		{
			if (std::getline(vector_stream, number, ','))
			{
				try
				{
					numbers.push_back(std::stof(number));
				}
				catch (const std::invalid_argument& e)
				{
					std::cout << "Error value type. IT IS NOT A FLOAT" << std::endl;
					numbers.push_back(0.0f);
				}
				catch (const std::out_of_range& e)
				{
					std::cout << "Error value type. IT OUT OF RANGE" << std::endl;
					numbers.push_back(0.0f);
				}
			}
			else
				numbers.push_back(0.0f);
		}
	}
	return numbers;
}
GLSLInfo ParserGLSL::Parser(std::string path)
{
	std::string line;
	std::ifstream shaderFile(path);
	std::string namevalue;
	GLSLInfo info{};
	bool fileCorrect = true;
	bool propertiesclassfind = false;
	bool endProperties = false;
	bool endParse = false;
	bool findProperties = false;
	bool findRenderPath = false;
	if (shaderFile.is_open())
	{
		while (std::getline(shaderFile, line) && endParse == false)
		{
			std::string trim = line;
			size_t commentPos = trim.find("//");
			if (commentPos != std::string::npos)
				trim = trim.substr(0, commentPos);
			size_t start = trim.find_first_not_of(" \t");
			if (start == std::string::npos)
				continue;

			size_t end = trim.find_last_not_of(" \t");
			trim = trim.substr(start, end - start + 1);
			if (endProperties || !propertiesclassfind)
			{
				size_t renderPathPos = trim.rfind("RenderPath");
				if (renderPathPos != std::string::npos)
				{
					size_t firstPosRenderPath = trim.find("\"");
					if (firstPosRenderPath == std::string::npos)
					{
						std::cout << "MISSING: \"" << std::endl;
						fileCorrect = false;
						continue;
					}
					if(emptyline(trim,  renderPathPos + std::string("RenderPath").size(), firstPosRenderPath))
					{
						size_t secondPosRenderPath = trim.rfind("\"");
						if (secondPosRenderPath == std::string::npos) 
						{
							std::cout << "MISSING: \"" << std::endl;
							fileCorrect = false;
							continue;
						}
						trim = trim.substr(firstPosRenderPath + 1, secondPosRenderPath - firstPosRenderPath - 1);
						trim.erase(std::remove_if(trim.begin(), trim.end(), [](char c) {return std::isspace((unsigned char)c); }), trim.end());
						if (trim == "Deferred")
						{
							info.RenderPath = 1;
						}
						else
							info.RenderPath = 0;
					}
					else
					{
						std::cout << "SYNTAXERROR:" << std::endl;
						fileCorrect = false;
						continue;
					}
				}
			}
			if (!propertiesclassfind)
			{
				if (trim == "Properties")
				{
					findProperties = true;
					continue;
				}
				if (findProperties)
				{
					if (trim == "{")
					{
						propertiesclassfind = true;
						continue;
					}
					findProperties = false;
					continue;
				}
				continue;
			}
			else
			{
				std::stringstream trimstream(trim);
				if (trim == "GLSLPROGRAM" && !endProperties)
				{
					std::cout << "SYNTAXERROR" << std::endl;
					fileCorrect = false;
					break;
				}
				if (trim == "GLSLPROGRAM" && endProperties)
				{
					endParse = true;
					break;
				}
				if (trim == "}")
				{
					endProperties = true;
				}
				else
				{
					PGLinfos::FieldInfo newinfo{};
					size_t parametrequal = trim.find("=");
					if (parametrequal == std::string::npos) continue;

					size_t parametrstart = trim.find("(", parametrequal);
					if (parametrstart == std::string::npos) continue;

					size_t paramenrend = trim.rfind(")");
					if (paramenrend == std::string::npos) continue;

					size_t separator = trim.find(",", parametrstart);
					if (separator == std::string::npos) continue;
					if (!emptyline(trim, parametrequal + 1, parametrstart) || !emptyline(trim, paramenrend + 1, trim.size()))
					{
						std::cout << "SYNTAX ERROR" << std::endl;
						fileCorrect = false;
						continue;
					}

					std::string type = trim.substr(parametrstart + 1, separator - parametrstart - 1);
					type.erase(std::remove_if(type.begin(), type.end(), [](char c) {return std::isspace((unsigned char)c); }), type.end());
					std::string value = trim.substr(separator + 1, paramenrend - separator - 1);
					value.erase(std::remove_if(value.begin(), value.end(), [](char c) {return std::isspace((unsigned char)c); }), value.end());
					std::cout << "New value: " << value << std::endl;
					if (type == "Texture2D")
					{
						newinfo.typefield = PGLinfos::FieldType::Sampler2D;
						newinfo.value = "white";
						if (value != "")
							newinfo.value = value;
					}
					else if (type == "TextureCube")
					{
						newinfo.typefield = PGLinfos::FieldType::SamplerCube;
						newinfo.value = "white";
						if (value != "")
							newinfo.value = value;
					}
					else if (type == "Float")
					{
						newinfo.typefield = PGLinfos::FieldType::Float;
						newinfo.value = 0.0f;
						if (value != "")
						{
							try
							{
								newinfo.value = stof(value);
							}
							catch (const std::invalid_argument& e)
							{
								std::cout << "Error value type. IT IS NOT A FLOAT" << std::endl;
								fileCorrect = false;
								continue;
							}
							catch (const std::out_of_range& e)
							{
								std::cout << "Error value type. IT OUT OF RANGE" << std::endl;
								fileCorrect = false;
								continue;
							}
						}
					}
					else if (type == "Color")
					{
						newinfo.typefield = PGLinfos::FieldType::Vector4;
						newinfo.value = GLSLParserMath::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
						newinfo.isColor = true;
						std::vector<float> vector4 = returnVectorClear(4, value);
						if (vector4.size() != 0)
							newinfo.value = GLSLParserMath::Vector4{ vector4[0], vector4[1], vector4[2], vector4[3] };
					}
					else if (type == "Bool")
					{
						newinfo.typefield = PGLinfos::FieldType::Bool;
						if (value == "true")
							newinfo.value = true;
						else
							newinfo.value = false;
					}
					else if (type == "Int")
					{
						newinfo.typefield = PGLinfos::FieldType::Int;
						newinfo.value = 0;
						if (value != "")
						{
							try
							{
								newinfo.value = stoi(value);
							}
							catch (const std::invalid_argument& e)
							{
								std::cout << "Error value type. IT IS NOT A FLOAT" << std::endl;
								fileCorrect = false;
								continue;
							}
							catch (const std::out_of_range& e)
							{
								std::cout << "Error value type. IT OUT OF RANGE" << std::endl;
								fileCorrect = false;
								continue;
							}
						}
					}
					else if (type == "Vector2")
					{
						newinfo.typefield = PGLinfos::FieldType::Vector2;
						newinfo.value = GLSLParserMath::Vector2{ 1.0f, 1.0f };
						std::vector<float> vector2 = returnVectorClear(2, value);
						if (vector2.size() != 0)
							newinfo.value = GLSLParserMath::Vector2{ vector2[0], vector2[1] };
					}
					else if (type == "Vector3")
					{
						newinfo.typefield = PGLinfos::FieldType::Vector3;
						newinfo.value = GLSLParserMath::Vector3{ 1.0f, 1.0f, 1.0f };
						std::vector<float> vector3 = returnVectorClear(3, value);
						if (vector3.size() != 0)
							newinfo.value = GLSLParserMath::Vector3{ vector3[0], vector3[1], vector3[2] };
					}
					else if (type == "Vector4")
					{
						newinfo.typefield = PGLinfos::FieldType::Vector4;
						newinfo.value = GLSLParserMath::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
						std::vector<float> vector4 = returnVectorClear(4, value);
						if (vector4.size() != 0)
							newinfo.value = GLSLParserMath::Vector4{ vector4[0], vector4[1], vector4[2], vector4[3] };
					}
					else
					{
						std::cout << "Unknown type" << std::endl;
						fileCorrect = false;
						continue;
					}

					size_t argument_one = trim.find("[");
					size_t argument_two = trim.find("]");
					if (argument_one != std::string::npos && argument_two != std::string::npos)
					{
						size_t argumentname_before_vector = trim.find("(", argument_one);
						size_t argumentname_before_vector1 = trim.find(")", argument_one);
						if (argumentname_before_vector != std::string::npos && argumentname_before_vector1 != std::string::npos && argumentname_before_vector < argument_two && argumentname_before_vector1 < argument_two)
						{
							if (!emptyline(trim, argumentname_before_vector1 + 1, argument_two) || !emptyline(trim, 0, argument_one))
							{
								std::cout << "SYNTAX ERROR" << std::endl;
								fileCorrect = false;
								continue;
							}
							std::string nameargument = trim.substr(argument_one + 1, argumentname_before_vector - argument_one - 1);
							nameargument.erase(std::remove_if(nameargument.begin(), nameargument.end(), [](char c) {return std::isspace((unsigned char)c); }), nameargument.end());
							if (newinfo.typefield == PGLinfos::FieldType::Float && nameargument == "Range")
							{
								std::vector<float> vector_argument = returnVectorDirty(2, trim, argument_one, argument_two);
								if (vector_argument.size() != 0)
								{
									newinfo.minrange = vector_argument[0];
									newinfo.maxrange = vector_argument[1];
									newinfo.useRange = true;
								}
								else
								{
									std::cout << "SYNTAX ERROR! IS NOT VECTOR2 IN RANGE" << std::endl;
									fileCorrect = false;
									continue;
								}
							}
						}
						else
						{
							std::string nameargument = trim.substr(argument_one + 1, argument_two - argument_one - 1);
							if (newinfo.isColor && nameargument == "RGB")
							{
								newinfo.RGB = true;
							}
						}
					}
					if (argument_two != std::string::npos)
					{
						namevalue = trim.substr(argument_two + 1, parametrequal - argument_two - 1);
						namevalue.erase(0, namevalue.find_first_not_of(" \t"));
						namevalue.erase(namevalue.find_last_not_of(" \t") + 1);
						if (namevalue.empty() || (!std::isalpha(namevalue[0]) && namevalue[0] != '_'))
						{
							std::cout << "SYNTAX ERROR" << std::endl;
							fileCorrect = false;
							continue;
						}
						else
						{
							bool correct = true;
							for (char& c : namevalue)
							{
								if (!std::isalnum((unsigned char)c) && c != '_')
								{
									std::cout << "SYNTAX ERROR" << std::endl;
									correct = false;
									break;
								}
							}
							if (!correct)
							{
								fileCorrect = false;
								continue;
							}
						}
					}
					else
					{
						namevalue = trim.substr(0, parametrequal);
						namevalue.erase(0, namevalue.find_first_not_of(" \t"));
						namevalue.erase(namevalue.find_last_not_of(" \t") + 1);
						if (namevalue.empty() || (!std::isalpha((unsigned char)namevalue[0]) && namevalue[0] != '_'))
						{
							std::cout << "SYNTAX ERROR" << std::endl;
							fileCorrect = false;
							continue;
						}
						else
						{
							bool correct = true;
							for (char& c : namevalue)
							{
								if (!std::isalnum((unsigned char)c) && c != '_')
								{
									std::cout << "SYNTAX ERROR" << std::endl;
									correct = false;
									break;
								}
							}
							if (!correct)
							{
								fileCorrect = false;
								continue;
							}
						}
					}
					namevalue.erase(0, namevalue.find_first_not_of(" \t"));
					namevalue.erase(namevalue.find_last_not_of(" \t") + 1);
					info.AddField(namevalue, newinfo);
					info.returnAllFieldsName().push_back(namevalue);
				}
			}
		}
	}
	else
	{
		fileCorrect = false;
		std::cout << "error file" << std::endl;
	}
	info.valid = fileCorrect;
	shaderFile.close();
	return info;
}
void GLSLInfo::AddField(const std::string& name, const PGLinfos::FieldInfo& info)
{
	fields[name] = info;
}
const std::unordered_map<std::string, PGLinfos::FieldInfo>& GLSLInfo::returnAllFields() const
{
	return fields;
}
std::vector<std::string>& GLSLInfo::returnAllFieldsName()
{
	return fieldsNames;
}


std::vector<std::string> AGLSLShaderToGLSL::includedFiles;
std::string AGLSLShaderToGLSL::returnAGLSLShaderToGLSL(const std::string& AGLSLFileSource)
{
	includedFiles.clear();
	return returnAGLSLShaderToGLSLFunction(AGLSLFileSource, false);
}
std::string AGLSLShaderToGLSL::returnAGLSLShaderToGLSLFunction(const std::string& AGLSLFileSource, bool isheader)
{
	std::string line;
	std::string sourcefile;
	std::stringstream streamfile(AGLSLFileSource);
	bool waitingGLSLPROGRAM = true;
	while (std::getline(streamfile, line))
	{
		size_t starttrim = line.find_first_not_of(" \t");
		if (starttrim == std::string::npos)
			continue;
		size_t endtrim = line.find_last_not_of(" \t");
		if (endtrim == std::string::npos)
			continue;
		std::string trim = line.substr(starttrim, endtrim - starttrim + 1);
		if (trim.find("//") == 0)
			continue;
		if (waitingGLSLPROGRAM && isheader == false)
		{
			if (trim == "GLSLPROGRAM")
			{
				waitingGLSLPROGRAM = false;
			}
			continue;
		}
		size_t includestart = trim.find("#include");
		if (includestart == 0)
		{
			size_t startSymbolofPath = trim.find("\"");
			if (startSymbolofPath == std::string::npos)
			{
				std::cout << "SYNTAXERROR startSymbolofPath" << std::endl;
				throw std::runtime_error("SYNTAXERROR");
			}
			size_t endSymbolofPath = trim.rfind("\"");
			if (endSymbolofPath == std::string::npos)
			{
				std::cout << "SYNTAXERROR endSymbolofPath" << std::endl;
				throw std::runtime_error("SYNTAXERROR");
			}
			if (!ParserGLSL::emptyline(trim, includestart + 8, startSymbolofPath))
			{
				std::cout << "SYNTAXERROR includefile" << std::endl;
				throw std::runtime_error("SYNTAXERROR");
			}
			if (!ParserGLSL::emptyline(trim, endSymbolofPath + 1, trim.size()))
			{
				std::cout << "SYNTAXERROR includefile" << std::endl;
				throw std::runtime_error("SYNTAXERROR");
			}
			std::string filepath = trim.substr(startSymbolofPath + 1, endSymbolofPath - startSymbolofPath - 1);
			if (std::find(includedFiles.begin(), includedFiles.end(), filepath) == includedFiles.end())
			{
				std::ifstream includeShader(filepath);
				if (includeShader.is_open())
				{
					std::stringstream strstream;
					strstream << includeShader.rdbuf();
					includedFiles.push_back(filepath);
					trim = returnAGLSLShaderToGLSLFunction(strstream.str(), true);
				}
				else
				{
					std::cout << "INCLUDE FILE IS NOT OPEN" << filepath << std::endl;
					throw std::runtime_error("INCLUDE FILE IS NOT OPEN");
				}
			}
			else
			{
				continue;
			}
		}
		sourcefile += trim + "\n";
	}
	if (waitingGLSLPROGRAM && isheader == false)
	{
		throw std::runtime_error("GLSLPROGRAM not found");
	}
	//std::cout << sourcefile << std::endl;
	return sourcefile;
}