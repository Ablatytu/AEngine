#pragma once
#include "../GameAPI/GameABI.h"

class Shader
{
public:
	Shader(AE_GAME_RESOURCES_INFO::AE_SHADER info);
	Shader() = default;
	AE_GAME_RESOURCES_INFO::AE_SHADER ABIShader = {};
};