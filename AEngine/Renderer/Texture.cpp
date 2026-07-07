#include "Texture.h"
#include "../core/ErrorManager.h"
#include <optional>
#define STB_IMAGE_IMPLEMENTATION
#include "../APIs/stb_image.h"

static GLenum GLDataFormat(Texture::DataFormat& dataformat)
{
	switch (dataformat)
	{
	case Texture::DataFormat::RGB:
		return GL_RGB;
	case Texture::DataFormat::RGB16F:
		return GL_RGB16F;
	case Texture::DataFormat::RGBA:
		return GL_RGBA;
	case Texture::DataFormat::RGBA16F:
		return GL_RGBA16F;
	case Texture::DataFormat::RG16F:
		return GL_RG16F;
	case Texture::DataFormat::RG:
		return GL_RG;
	default:
		return GL_RGB;
	}
}
static GLenum GLFilterFormat(Texture::FilterType& filterformat)
{
	switch (filterformat)
	{
	case Texture::FilterType::NEAREST:
		return GL_NEAREST;
	case Texture::FilterType::REPEAT:
		return GL_REPEAT;
	case Texture::FilterType::CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
	case Texture::FilterType::LINEAR:
		return GL_LINEAR;
	default:
		return GL_NEAREST;
	}
}

void Texture::CreateColorTexture(std::string name)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char px[3];
	if(name == "blue")
	{
		px[0] = 0; px[1] = 0; px[2] = 255;
	}
	else if (name == "normal")
	{
		px[0] = 128; px[1] = 128; px[2] = 255;
	}
	else if (name == "black")
	{
		px[0] = 0; px[1] = 0; px[2] = 0;
	}
	else
	{
		px[0] = 255; px[1] = 255; px[2] = 255;
	}
	width = 1; height = 1; nrChannels = 3;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, px);
	glGenerateMipmap(GL_TEXTURE_2D);
}
void Texture::LoadTexture(const char* path, bool linear, std::optional<DataFormat> imageFormat, std::optional<DataFormat> dataFormat, std::vector<FilterType> filter1, std::vector<FilterType> filter2)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, filter1.size() == 2 ? GLFilterFormat(filter1[0]) : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, filter1.size() == 2 ? GLFilterFormat(filter1[1]) : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter2.size() == 2 ? GLFilterFormat(filter2[0]) : GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter2.size() == 2 ? GLFilterFormat(filter2[1]) : GL_LINEAR);
	data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)
		{
			format = GL_RED;
		}
		else if (nrChannels == 3)
		{
			format = GL_RGB;
			if (!linear)
				format = GL_SRGB;
		}
		else if (nrChannels == 4)
		{
			format = GL_RGBA;
			if (!linear)
				format = GL_SRGB_ALPHA;
		}
		if (imageFormat)
			format = GLDataFormat(*imageFormat);
		GLenum externalFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		if (dataFormat)
			externalFormat = GLDataFormat(*dataFormat);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		CriticalGameError("Texture not found!");
		return;
	}
}
void Texture::CreateTexture(unsigned int width, unsigned int height, DataFormat iternalFormat, DataFormat dataformat, FilterType filter1, FilterType filter2, void* data)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GLDataFormat(iternalFormat), width, height, 0, GLDataFormat(dataformat), GL_FLOAT, data == nullptr ? NULL : data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLFilterFormat(filter1));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLFilterFormat(filter1));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLFilterFormat(filter2));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLFilterFormat(filter2));
}
unsigned char* Texture::GetData()
{
	return data;
}
Texture::~Texture()
{
	if (data)
		stbi_image_free(data);
	glDeleteTextures(1, &texture);
}
unsigned int Texture::GetTexturePixels()
{
	return texture;
}