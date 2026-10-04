#pragma once
#include <vector>
#include "../core/AEngineAPI.h"
class SpriteRenderer2D;
class Camera;

struct AENGINE_API RenderContext2D
{
	bool perspective = false;
	std::vector<SpriteRenderer2D*> sprites;
	Camera* camera = nullptr;
	int width = 0, height = 0;
};