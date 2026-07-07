#pragma once
#include <string>
#include "../core/AEngineAPI.h"

class AENGINE_API HDRTexture
{
private:
	float* data;
	unsigned int texture;
public:
	int width, height, nrChannels;
	std::string path;
	void LoadTexture(const char* path);
	float* GetData();
	unsigned int GetTexturePixels();
	~HDRTexture();
};