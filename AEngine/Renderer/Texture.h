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
public:
	using DataFormat = DataFormat;
	using FilterType = FilterType;

	int width = 0, height = 0, nrChannels = 0;
	std::string path;
	void CreateColorTexture(std::string name);
	void LoadTexture(const char* path, bool _linear = false, std::optional<DataFormat> _imageFormat = std::nullopt, std::optional<DataFormat> _dataFormat = std::nullopt, std::vector<FilterType> filter1 = {}, std::vector<FilterType> filter2 = {});
	void CreateTexture(unsigned int width, unsigned int height, DataFormat _iternalFormat , DataFormat _dataformat, FilterType filter1, FilterType filter2, void* data = nullptr);
	void SetLinear(bool _linear);
	int GetWidth();
	int GetHeight();
	bool GetLinear();
	unsigned char* GetData();
	unsigned int GetTexturePixels();
	~Texture();
private:
	unsigned char* data = nullptr;;
	unsigned int texture = 0;
	bool linear = false;
	unsigned int imageFormat = 0;
	unsigned int dataFormat = 0;

};