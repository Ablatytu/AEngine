#pragma once
#include <glad/glad.h>
#include <string>
#include <optional>

class Texture
{
public:
	int width, height, nrChannels;
	unsigned int texture;
	std::string path;
	unsigned char* data;
	void CreateColorTexture(std::string name);
	void CreateTexture(const char *path, bool linear);
	~Texture();
};