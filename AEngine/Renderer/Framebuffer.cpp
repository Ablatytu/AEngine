#include "Framebuffer.h"
#include "Shader.h"
#include <glad/glad.h>
#include "../core/Window.h"
#include "../core/Application.h"
#include "../scene/SceneManager.h"
#include "../core/ErrorManager.h"
#include "../scene/AssetManager.h"

static void GetFormat(Framebuffer::TypeFrameBuffer typeFrameBuffer, GLenum& internalFormat, GLenum& dataFormat, GLenum& dataType)
{
	switch (typeFrameBuffer)
	{
	case Framebuffer::TypeFrameBuffer::LDR:
		internalFormat = GL_RGB;
		dataFormat = GL_RGB;
		dataType = GL_UNSIGNED_BYTE;
		break;
	case Framebuffer::TypeFrameBuffer::RGBA:
		internalFormat = GL_RGBA;
		dataFormat = GL_RGBA;
		dataType = GL_UNSIGNED_BYTE;
		break;
	case Framebuffer::TypeFrameBuffer::HDR:
		internalFormat = GL_RGB16F;
		dataFormat = GL_RGB;
		dataType = GL_FLOAT;
		break;
	case Framebuffer::TypeFrameBuffer::R16F:
		internalFormat = GL_R16F;
		dataFormat = GL_RED;
		dataType = GL_FLOAT;
		break;
	case Framebuffer::TypeFrameBuffer::RGBA16F:
		internalFormat = GL_RGBA16F;
		dataFormat = GL_RGBA;
		dataType = GL_FLOAT;
		break;
	default:
		internalFormat = GL_RGB;
		dataFormat = GL_RGB;
		dataType = GL_UNSIGNED_BYTE;
		break;
	}
}
static void GetFilter(Framebuffer::TypeFilter typefilter, GLenum& filter)
{
	switch (typefilter)
	{
	case Framebuffer::TypeFilter::Liner:
		filter = GL_LINEAR;
		break;
	case Framebuffer::TypeFilter::Nearest:
		filter = GL_NEAREST;
		break;
	default:
		filter = GL_LINEAR;
		break;
	}
}
Framebuffer::Framebuffer(Shader* shader, const std::vector<TypeFrameBuffer>& typeFrameBuffer, bool resizable, int width, int height, const std::vector<TypeFilter>& typeFilter)
{
	this->typeFrameBuffer = typeFrameBuffer;
	this->resizable = resizable;
	this->typeFilter = typeFilter;
	unsigned int _width = width == -1 ? Application::Get().window.screenX : width;
	unsigned int _height = height == -1 ? Application::Get().window.screenY : height;
	GLenum internalFormat;
	GLenum dataFormat;
	GLenum dataType;
	GLenum typefilter;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	float initialExposure = 1.0f;
	ColorBuffer.resize(typeFrameBuffer.size());
	std::vector<unsigned int> attachments;
	attachments.resize(typeFrameBuffer.size());
	for (unsigned int i = 0; i < typeFrameBuffer.size(); i++)
	{
		GetFormat(typeFrameBuffer[i], internalFormat, dataFormat, dataType);
		GetFilter(typeFilter[i], typefilter);
		glGenTextures(1, &ColorBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, ColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, dataFormat, dataType, typeFrameBuffer[i] == TypeFrameBuffer::R16F ? &initialExposure : NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, typefilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, typefilter);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColorBuffer[i], 0);
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glDrawBuffers(typeFrameBuffer.size(), attachments.data());
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CriticalGameError("Framebuffer error");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	screenShader = shader;

	unsigned int quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	if (shader)
	{
		screenShader->Update();
		screenShader->setInt("ScreenTexture", 0);
	}
	glBindVertexArray(0);
}
Shader* Framebuffer::GetShader()
{
	return screenShader;
}
void Framebuffer::ChangeShader(Shader* shader)
{
	screenShader = shader;
}
void Framebuffer::UseShader()
{
	if(screenShader)
		screenShader->Update();
}
void Framebuffer::Drawframebuffer()
{
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}
void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//std::vector<GLenum> attachments;
		//attachments.resize(ColorBuffer.size());

		//for (unsigned int i = 0; i < ColorBuffer.size(); i++)
		//	attachments[i] = GL_COLOR_ATTACHMENT0 + i;

		//glDrawBuffers((GLsizei)attachments.size(), attachments.data());
}
void Framebuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
unsigned int Framebuffer::GetFBO()
{
	return framebuffer;
}
void Framebuffer::Resize(int width, int height)
{
	if ((width <= 0 || height <= 0) || !resizable)
		return;
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	GLenum internalFormat;
	GLenum dataFormat;
	GLenum dataType;
	GLenum filtertype;
	for (unsigned int i = 0; i < typeFrameBuffer.size(); i++)
	{
		GetFormat(typeFrameBuffer[i], internalFormat, dataFormat, dataType);
		GetFilter(typeFilter[i], filtertype);
		glBindTexture(GL_TEXTURE_2D, ColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, dataType, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtertype);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtertype);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CriticalGameError("Framebuffer error");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Framebuffer::GetColorBuffer(unsigned int MCBNumber)
{
	return ColorBuffer[MCBNumber];
}