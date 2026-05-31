#include "Texture.h"
#include "../core/ErrorManager.h"
#include <optional>
#define STB_IMAGE_IMPLEMENTATION
#include "../APIs/stb_image.h"

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
void Texture::CreateTexture(const char* path, bool linear)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
		GLenum externalFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		CriticalGameError("Texture not found!");
		return;
	}
}
Texture::~Texture()
{
	if (data)
		stbi_image_free(data);
	glDeleteTextures(1, &texture);
}