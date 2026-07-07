#pragma once
#include <unordered_map>
#include "../core/AEngineAPI.h"
enum class TypeTexture
{
	right = 0,
	left = 1,
	top = 2,
	bottom = 3, 
	front = 4, 
	back = 5
};
class Texture;

class AENGINE_API Cubemap
{
private:
	unsigned int textureID;
public:
	unsigned int size;
	std::unordered_map<TypeTexture, Texture*> textures;
	Cubemap(unsigned int size, bool HDR = false, bool mipmap = false);
	void CreateMipmap();
	unsigned int GetTextureID();
	void ChangeTexture(TypeTexture, Texture*);
};