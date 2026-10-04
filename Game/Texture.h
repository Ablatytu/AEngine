#pragma once
#include "../GameAPI/GameABI.h"

class Texture
{
public:
	Texture() = default;
	Texture(AE_GAME_RESOURCES_INFO::AE_TEXTURE texture);
	AE_GAME_RESOURCES_INFO::AE_TEXTURE ABITexture{};
};