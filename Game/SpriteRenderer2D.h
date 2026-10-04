#pragma once
#include "AComponent.h"
#include "Vector4.h"
class Texture;

class SpriteRenderer2D final : public AComponent
{
public:
	SpriteRenderer2D() = default;

	void setTexture(Texture texture);
	void setColor(Vector4 color);
};