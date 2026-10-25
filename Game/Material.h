#pragma once
#include "../GameAPI/GameABI.h"
#include <variant>

class Shader;
class Texture;

class Material
{
public:
	Material(const char* name);
	Material(AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material);
	Material() = default;
	void SetTexture(const char* nameField, Texture& texture);
	void Set(const char* nameField, std::variant<int, float, bool> number);
	AE_GAME_RESOURCES_INFO::AE_MATERIAL ABIMaterial{};
};