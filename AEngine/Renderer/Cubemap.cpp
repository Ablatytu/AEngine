#include "Cubemap.h"
#include "../Renderer/Texture.h"
#include <glad/glad.h>

void Cubemap::ChangeTexture(TypeTexture typeTexture, Texture* texture)
{
	if (!texture)
		return;

	if (texture->GetWidth() != size || texture->GetHeight() != size)
		return;

	textures[typeTexture] = texture;
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(typeTexture);
	glTexSubImage2D(face, 0, 0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, texture->GetData());
	if (mipmap)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
Cubemap::Cubemap(unsigned int _size, bool _HDR, bool _mipmap)
{
	HDR = _HDR;
	size = _size;
	mipmap = _mipmap;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	if (HDR)
	{
		std::vector<float> blackPixels(_size * _size * 3, 0.0f);
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, HDR ? GL_RGB16F : GL_RGB, _size, _size, 0, GL_RGB, HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, blackPixels.data());
		}
	}
	else
	{
		std::vector<unsigned char> blackPixels(_size * _size * 3, 0);
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, HDR ? GL_RGB16F : GL_RGB, _size, _size, 0, GL_RGB, HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, blackPixels.data());
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmap? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	if (_mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
void Cubemap::ChangeSize(unsigned int _size)
{
	if (_size == size) return;
	textures.clear();
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	if (HDR)
	{
		std::vector<float> blackPixels(_size * _size * 3, 0.0f);
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, HDR ? GL_RGB16F : GL_RGB, _size, _size, 0, GL_RGB, HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, blackPixels.data());
		}
	}
	else
	{
		std::vector<unsigned char> blackPixels(_size * _size * 3, 0);
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, HDR ? GL_RGB16F : GL_RGB, _size, _size, 0, GL_RGB, HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, blackPixels.data());
		}
	}
	size = _size;
	if (mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
void Cubemap::CreateMipmap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	mipmap = true;
}
unsigned int Cubemap::GetTextureID()
{
	return textureID;
}