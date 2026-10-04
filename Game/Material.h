#pragma once
#include "../GameAPI/GameABI.h"

class Shader;
class Texture;

class Material
{
public:
	Material(const char* name);
	Material(AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material);
	Material() = default;
	void SetTexture(const char* nameFiel, Texture& texture);
	AE_GAME_RESOURCES_INFO::AE_MATERIAL ABIMaterial{};
};