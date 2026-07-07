#include "Cubemap.h"
#include "../Renderer/Texture.h"
#include <glad/glad.h>

void Cubemap::ChangeTexture(TypeTexture typeTexture, Texture* texture)
{
	textures[typeTexture] = texture;
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(typeTexture);
	glTexSubImage2D(face, 0, 0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, texture->GetData());
}
Cubemap::Cubemap(unsigned int _size, bool HDR, bool mipmap)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, HDR ? GL_RGB16F : GL_RGB, _size, _size, 0, GL_RGB, HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, nullptr);
	size = _size;
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmap? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
void Cubemap::CreateMipmap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
unsigned int Cubemap::GetTextureID()
{
	return textureID;
}