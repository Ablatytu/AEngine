#pragma once
#include <glad/glad.h>
#include <string>
#include <optional>
#include <vector>
#include "../core/AEngineAPI.h"

enum class FilterType
{
	NEAREST,
	REPEAT,
	CLAMP_TO_EDGE,
	LINEAR
};
enum class DataFormat
{
	RGBA16F,
	RGB16F,
	RGBA,
	RGB,
	RG16F,
	RG,
	NONE
};

class AENGINE_API Texture
{
private:
	unsigned char* data;
	unsigned int texture;
public:
	using DataFormat = DataFormat;
	using FilterType = FilterType;

	int width, height, nrChannels;
	std::string path;
	void CreateColorTexture(std::string name);
	void LoadTexture(const char* path, bool linear, std::optional<DataFormat> imageFormat = std::nullopt, std::optional<DataFormat> dataFormat = std::nullopt, std::vector<FilterType> filter1 = {}, std::vector<FilterType> filter2 = {});
	void CreateTexture(unsigned int width, unsigned int height, DataFormat iternalFormat , DataFormat dataformat, FilterType filter1, FilterType filter2, void* data = nullptr);
	unsigned char* GetData();
	unsigned int GetTexturePixels();
	~Texture();
};