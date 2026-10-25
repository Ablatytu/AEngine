#pragma once
#include "AComponentRef.h"

class RenderImage;
class Material;
class Camera; 

class GraphicsAPI
{
public:
	static void Blit(RenderImage* source, RenderImage* destination, Material* mat, Camera* cam);
};