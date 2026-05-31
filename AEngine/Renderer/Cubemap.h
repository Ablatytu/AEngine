#pragma once
#include <unordered_map>
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

class Cubemap
{
public:
	unsigned int textureID;
	unsigned int size;
	std::unordered_map<TypeTexture, Texture*> textures;
	Cubemap(unsigned int size);
	void ChangeTexture(TypeTexture, Texture*);
};