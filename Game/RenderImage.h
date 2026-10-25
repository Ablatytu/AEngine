#pragma once
#include "../GameAPI/GameABI.h"

class RenderImage
{
public:
	RenderImage(AE_GAME_RESOURCES_INFO::AE_RENDERIMAGE ae_renderimage);
	AE_GAME_RESOURCES_INFO::AE_RENDERIMAGE ABIRenderImage = {};
};