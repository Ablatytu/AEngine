#include "HDRTexture.h"
#include "../core/ErrorManager.h"
#include <glad/glad.h>
#include "../APIs/stb_image.h"

float* HDRTexture::GetData()
{
	return data;
}
unsigned int HDRTexture::GetTexturePixels()
{
	return texture;
}
void HDRTexture::LoadTexture(const char* path)
{
	stbi_set_flip_vertically_on_load(true);
	data = stbi_loadf(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		CriticalGameError("HDRTexture not found!");
		return;
	}
	stbi_set_flip_vertically_on_load(false);
}
HDRTexture::~HDRTexture()
{
	if (data)
		stbi_image_free(data);
	glDeleteTextures(1, &texture);
}